//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#ifndef LGBENCH_H
#define LGBENCH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include <chrono>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#ifndef __linux__
#include <mach/mach.h>
#endif

#include "likely.hpp"

#include "linux-perf-events.hpp"

class Lbench {
private:
  LinuxEvents<PERF_TYPE_HARDWARE> linux;

  int parseLine(char *line) const {
    // This assumes that a digit will be found and the line ends in " Kb".
    int         i = strlen(line);
    const char *p = line;
    while(*p < '0' || *p > '9')
      p++;
    line[i - 3] = '\0';
    i           = atoi(p);
    return i;
  }

  int getValue() const { // Note: this value is in KB!
#ifdef __linux__
    FILE *file   = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while(fgets(line, 128, file) != nullptr) {
      if(strncmp(line, "VmRSS:", 6) == 0) {
        result = parseLine(line);
        break;
      }
    }
    fclose(file);
    return result;
#else
    task_vm_info_data_t vmInfo;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    kern_return_t kernelReturn = task_info(mach_task_self(), TASK_VM_INFO, (task_info_t) &vmInfo, &count);
    if(kernelReturn == KERN_SUCCESS) {
      return (int) vmInfo.phys_footprint / (1024 * 1024);
    }
    return 0;
#endif
  }

  static inline bool perf_setup   = false;
  static inline bool perf_enabled = false;
  pid_t perf_pid=0;

protected:
  typedef std::chrono::time_point<std::chrono::system_clock> Time_Point;
  struct Time_Sample {
    Time_Point  tp;
    int         mem;
    size_t      ninst;
    size_t      ncycles;
    size_t      nbr_misses;
    size_t      nmem_misses;
    std::string name;
  };
  std::vector<Time_Sample> record;
  const std::string        sample_name;
  Time_Point               start_time;
  int                      start_mem;
  bool                     end_called;

  void perf_start(const std::string& name) {
    if (unlikely(!perf_setup)) {
      const char *do_perf = getenv("LGBENCH_PERF");
      if (do_perf==nullptr) {
        perf_enabled = false;
      }else if (do_perf[0]=='0') {
        perf_enabled = false;
      }else{
        perf_enabled = true;
      }
      if (perf_enabled && access("/usr/bin/perf", X_OK) == -1) {
        std::cerr << "ERROR: lgbench could not find /usr/bin/perf in system\n";
        exit(-3);
      }

      perf_setup=true;
    }
    if (!perf_enabled)
      return;

    std::string filename = name.find(".data") == std::string::npos ? (name + ".data") : name;

    std::stringstream s;
    s << getpid();
    perf_pid = fork();
    if (perf_pid == 0) {
      auto fd=open("/dev/null",O_RDWR);
      dup2(fd,1);
      dup2(fd,2);
      exit(execl("/usr/bin/perf","perf","record","-o",filename.c_str(),"-p",s.str().c_str(),nullptr));
    }
  }

  void perf_stop() {
    if (!perf_enabled)
      return;
    // Kill profiler
    kill(perf_pid,SIGINT);
    waitpid(perf_pid,nullptr,0);
  }

public:
  explicit Lbench(const std::string &name)
      : sample_name(name) {
    end_called = false;
    perf_start(name);

    const std::vector<int> evts{
#ifdef __linux__
      PERF_COUNT_HW_CPU_CYCLES,
      PERF_COUNT_HW_INSTRUCTIONS,
      PERF_COUNT_HW_BRANCH_MISSES,
      PERF_COUNT_HW_CACHE_REFERENCES
#endif
    };
    linux.setup(evts);

    start();
  }

  ~Lbench() {
    if(end_called)
      return;
    end();
    perf_stop();
  }

  void start() {
    start_time = std::chrono::system_clock::now();
    start_mem  = getValue();
    linux.start();
  }

  void sample(const std::string &name) {
    std::vector<uint64_t> stats(4);
    linux.sample(stats);

    Time_Sample s;
    s.tp          = std::chrono::system_clock::now();
    s.mem         = getValue();
    s.ncycles     = stats[0];
    s.ninst       = stats[1];
    s.nbr_misses  = stats[2];
    s.nmem_misses = stats[3];
    s.name        = name;

    record.push_back(s);
  }

  double get_secs() const {
    Time_Point tp = std::chrono::system_clock::now();

    Time_Point prev     = start_time;
    std::chrono::duration<double> t = tp - start_time;
    return t.count();
  }

  void end() {
    if (end_called)
      return;
    end_called = true;

    Time_Point tp = std::chrono::system_clock::now();

    Time_Point prev     = start_time;
    int        prev_mem = start_mem;

    for(const auto &s : record) {
      std::chrono::duration<double> t = s.tp - prev;

      if(s.name == "end" && t.count() < 0.01)
        continue;

      int m;
      if(prev_mem > s.mem)
        m = prev_mem - s.mem;
      else
        m = s.mem - prev_mem;

#if 0
      std::cerr << s.name << " secs=" << t.count();
      if (s.ncycles) {
        std::cerr
          << ":IPC=" << ((double)s.ninst) / (s.ncycles+1)
          << ":BR MPKI=" << ((double)s.nbr_misses*1000 ) / (s.ninst+1)
          << ":L2 MPKI=" << ((double)s.nmem_misses*1000) / (s.ninst+1);
      }
      std::cerr << m << ":KB delta " << s.mem << "KB abs\n";
#endif

      prev     = s.tp;
      prev_mem = s.mem;
    }
    std::vector<uint64_t> stats(4);
    linux.stop(stats);
    linux.close();

    std::chrono::duration<double> t = tp - start_time;
    std::stringstream sstr;
    sstr
      << std::setw(28) << std::left << sample_name 
      << " secs="      << std::setw(15) << t.count()
      << " IPC="       << std::setw(6)  << ((double)stats[1]) / (stats[0]+1)
      << " BR_MPKI="   << std::setw(6)  << ((double)stats[2]*1000) / (stats[1]+1)
      << " L2_MPKI="   << std::setw(6)  << ((double)stats[3]*1000) / (stats[1]+1)
      << "\n";
    // std::cerr << sstr.str();

    int tfd = ::open("lbench.trace",O_CREAT|O_RDWR|O_APPEND, 0644);

    if (tfd>=0) {
      write(tfd, sstr.str().data(), sstr.str().size());
      close(tfd);
    }
  }
};
#endif
