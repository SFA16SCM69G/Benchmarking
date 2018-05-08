Program name: MyCPUBench.c
#######################################################################################################
Compile program:
Before compiliing, make sure a folder named "output" is contained in the same directory of MyCPUBench.c
Stardard commands:
Step1: cd directory(the directory contains MyCPUBench.c)
Step2: gcc -pthread -o exec_name MyCPUBench.c
Example commands:
Step1: cd cs553-pa1/cpu
Step2: gcc -pthread -o mycpu MyCPUBench.c
#######################################################################################################
Run program:
Standard commands:
./exec_name <number of threads (1,2,4)>  <workload type (QP, HP, SP, DP)>
Example commands:
./mycpu 2 SP

#######################################################################################################
Results of the program contains the execution time and processor speed(GigaFLOPS).
Results will be saves as .txt files in output folder
