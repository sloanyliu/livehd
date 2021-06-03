import csv

time_data_list = []
ipc_data_list = []

start = 5
end = 50
tests_per_run = end - start + 1 
number_of_runs = 20
filename = 'pstr_perf_analysis/pstr-data2.trace'
time_csv = 'pstr_perf_analysis/time_plot_data2.csv'
ipc_csv = 'pstr_perf_analysis/ipc_plot_data2.csv'

with open(filename, 'r') as file:
    for x in range(0,number_of_runs):
        for i in range(0,tests_per_run):                       # iterate through number of tests
            for j in range(0,2):                               # first is pstr, then std
                line = file.readline()                         # save the line
                if (line[0] == '<' or len(line) == 0 or len(line) == 1):  # skipping delimiter lines
                    line = file.readline()
                time_indx = line.find("secs")                  # find where secs starts
                ipc_indx = line.find("IPC")                    # find where IPC starts
                end_indx = line.find("BR_MPKI")                # find where BR_MPKI starts
                time_var =  (line[time_indx+5 : ipc_indx-1]).strip() # no spaces
                ipc_var = (line[ipc_indx+4 : end_indx-1]).strip()    # no spaces
                if (j == 0):                                   # pstr
                    if (x == 0):                               # first pass
                        time_data_list.append([i+start])
                        ipc_data_list.append([i+start])
                        time_data_list[i].append(float(time_var))
                        ipc_data_list[i].append(float(ipc_var))
                    else:                                      # subsequent passes
                        time_data_list[i][j+1] = time_data_list[i][j+1] + float(time_var)
                        ipc_data_list[i][j+1] = ipc_data_list[i][j+1] + float(ipc_var)
                        if (x == number_of_runs-1):            # last run means get avg
                            time_data_list[i][j+1] = time_data_list[i][j+1] / number_of_runs
                            ipc_data_list[i][j+1] = ipc_data_list[i][j+1] / number_of_runs
                elif (j == 1):                                 #std
                    if (x == 0):
                        time_data_list[i].append(float(time_var))
                        ipc_data_list[i].append(float(ipc_var))
                    else:
                        time_data_list[i][j+1] = time_data_list[i][j+1] + float(time_var)
                        ipc_data_list[i][j+1] = ipc_data_list[i][j+1] + float(ipc_var)
                        if (x == number_of_runs-1):            # last run means get avg
                            time_data_list[i][j+1] = time_data_list[i][j+1] / number_of_runs
                            ipc_data_list[i][j+1] = ipc_data_list[i][j+1] / number_of_runs

time_data_list.insert(0,["Max Length", "mmap_lib::str", "std::string"])
ipc_data_list.insert(0,["Max Length", "mmap_lib::str", "std::string"])

with open(time_csv, 'w', newline='') as file1:
    writer1 = csv.writer(file1)
    writer1.writerows(time_data_list)

with open(ipc_csv, 'w', newline='') as file2:
    writer2 = csv.writer(file2)
    writer2.writerows(ipc_data_list)




