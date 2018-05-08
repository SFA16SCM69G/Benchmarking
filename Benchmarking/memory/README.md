Program name: MyRAMBench.c
#######################################################################################################
Compile program:
Before compiliing, make sure a folder named "output" is contained in the same directory of MyRAMBench.c
Stardard commands:
Step1: cd directory(the directory contains MyRAMBench.c)
Step2: gcc -pthread -o exec_name MyRAMBench.c
Example commands:
Step1: cd cs553-pa1/memory
Step2: gcc -pthread  -o myram MyRAMBench.c
#######################################################################################################
Run program:
Standard commands:
For throughput: ./exec_name <number of threads (1,2,4)>  <access pattern (RWS,RWR)> <block size (1,1000,10000)>(For the block size paramter, 1 means 1KB, 1000 means 1MB, 10000 means 10MB)
For latency: ./exec_name <number of threads (1,2,4)>  <access pattern (RWS,RWR)> -l

Example commands:
./myram 2 RWR 1000
./myram 1 RWS -l
#######################################################################################################
Results of the program contains the execution time and throughput(GB/sec) or latency(us).
Results will be saves as .txt files in output folder.
