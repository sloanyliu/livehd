#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

echo "verilog.sh running in "$(pwd)

LGSHELL=./bazel-bin/main/lgshell

if [ ! -x $LGSHELL ]; then
  if [ -x ./main/lgshell ]; then
    LGSHELL=./main/lgshell
    echo "lgshell is in $(pwd)"
  else
    echo "FAILED: verilog.sh could not find lgshell binary in $(pwd)";
  fi
fi

LGCHECK=./inou/yosys/lgcheck

rm -rf ./logs

inputs=inou/yosys/tests/*.v
long=""
if [ "$1" == "long" ]; then
  long="true"
  shift
fi
fixme=""
if [ "$1" != "" ]; then
  long="true"
  fixme="true"
  inputs=""
  while [ "$1" != "" ]; do
    inputs+=" "$1
    shift
  done
  echo "verilog.sh inputs: ${inputs}"
fi

pass=0
fail=0
fail_list=""
rm -rf tmp_yosys_mix
mkdir -p tmp_yosys_mix
for full_input in ${inputs}
do
  STARTTIME=$SECONDS
  #echo "starting test "${input}" at "$(/usr/bin/date)
  input=$(basename ${full_input})
  echo ${full_input}
  base=${input%.*}

  if [[ $input =~ "long_" ]]; then
    if [[ $long == "" ]]; then
      echo "Skipping long test for "$base
      continue
    fi
    base=${base:5}
  else
    if [[ $long == "true" && $fixme != "true" ]]; then
      echo "Skipping short test for "$base
      continue
    fi
  fi
  if [[ $input =~ "fixme_" ]]; then
    if [[ $fixme == "" ]]; then
      echo "Skipping fixme test for "$base
      echo "PLEASE: Somebody fix this!!!"
      continue
    fi
    base=${base:6}
  fi
  if [[ $input =~ "nocheck_" ]]; then
    base=${base:8}
  fi

  rm -rf lgdb_yosys tmp_yosys
  mkdir -p tmp_yosys

  #echo "inou.yosys.tolg path:lgdb_yosys top:${base} files:"${full_input}  | ${LGSHELL} -q >tmp_yosys/${input}.log 2>tmp_yosys/${input}.err
  echo "inou.verilog path:lgdb_yosys top:${base} files:${full_input} |> pass.compiler "  | ${LGSHELL} -q >tmp_yosys/${input}.log 2>tmp_yosys/${input}.err
  if [ $? -eq 0 ]; then
    echo "Successfully created graph from ${input}"
  else
    echo "FAIL: lgyosys parsing terminated with an error (testcase ${input})"
    ((fail++))
    fail_list+=" "$base
    continue
  fi
  LC=$(grep -iv Warning tmp_yosys/${input}.err | grep -v perf_event | grep -v "recommended to use " | wc -l | cut -d" " -f1)
  if [[ $LC -gt 0 ]]; then
    echo "FAIL: Faulty $LC err verilog file tmp_yosys/${input}.err"
    ((fail++))
    fail_list+=" "$base
    continue
  fi
  LC=$(grep -i signal tmp_yosys/${input}.log | wc -l | cut -d" " -f1)
  if [[ $LC -gt 0 ]]; then
    echo "FAIL: Faulty $LC log verilog file tmp_yosys/${input}.log"
    ((fail++))
    fail_list+=" "$base
    continue
  fi

  echo "lgraph.match path:lgdb_yosys |> pass.cprop |> inou.cgen.verilog odir:tmp_yosys" | ${LGSHELL} -q 2>tmp_yosys/${input}.err
  #echo "lgraph.match path:lgdb_yosys |> pass.cprop |> inou.yosys.fromlg odir:tmp_yosys" | ${LGSHELL} -q 2>tmp_yosys/${input}.err
  LC=$(grep -iv Warning tmp_yosys/${input}.err | grep -v perf_event | grep -v "recommended to use " | grep -v "IPC=" | wc -l | cut -d" " -f1)
  if [[ $LC -gt 0 ]]; then
    echo "FAIL: Faulty $LC err verilog file tmp_yosys/${input}.err"
    ((fail++))
    fail_list+=" "$base
    continue
  fi
  if [ $? -eq 0 ]; then
    echo "Successfully created verilog from graph ${input}"
  else
    echo "yosys -g"${base} -h -d
    echo "FAIL: verilog generation terminated with an error (testcase ${input})"
    ((fail++))
    fail_list+=" "$base
    continue
  fi
  $(cat tmp_yosys/*.v >tmp_yosys_mix/all_${base}.v)

  if [[ $input =~ "nocheck_" ]]; then
    LC=$(wc -l tmp_yosys_mix/all_${base}.v | cut -d" " -f1)
    echo "Skipping check for $base LC:"$LC
    if [[ $LC -lt 2 ]]; then
      echo "FAIL: Generated verilog file tmp_yosys_mix/all_${base}.v is too small"
      ((fail++))
      fail_list+=" "$base
      continue
    fi
  else
    ${LGCHECK} --implementation=tmp_yosys_mix/all_${base}.v --reference=${full_input} --top=${base}
    if [ $? -eq 0 ]; then
      echo "Successfully matched generated verilog with original verilog (${full_input})"
    else
      echo "FAIL: circuits are not equivalent (${full_input})"
      ((fail++))
      fail_list+=" "$base
      continue
    fi
  fi

  ((pass++))

  ENDTIME=$SECONDS
  echo "perf: takes $(($ENDTIME - $STARTTIME)) for top:"${base}
done

FAIL=$fail
for job in $(jobs -p)
do
  echo $job
  wait $job || let "FAIL+=1"
done

if [ $FAIL -eq 0 ]; then
  echo "SUCCESS: pass:${pass} tests without errors"
  exit 0
else
  echo "FAIL: ${pass} tests passed but ${fail} failed verification: ${fail_list}"
  exit 1
fi

