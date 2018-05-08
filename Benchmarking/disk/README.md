Program name: MyDiskBench.c and ini_read.c
#######################################################################################################
Compile program:
Before compiliing, make sure a folder named "output" and a folder named "file" are contained in the same directory of MyDiskBench.c
Stardard commands:
Step1: cd directory(the directory contains MyDiskBench.c and ini_read.c)
Step2: gcc -pthread -D_FILE_OFFSET_BITS=64 -o exec_name MyDiskBench.c
           gcc -o exec_name ini_read.c
Example commands:
Step1: cd cs553-pa1/disk
Step2: gcc -pthread -D_FILE_OFFSET_BITS=64 -o mydisk MyDiskBench.c
           gcc -o ini ini_read.c
#######################################################################################################
Run program:
Standard commands:
Initialize the files for read first: ./ini
After running ini, a file named R.txt, size of 10GB and a file named R_l.txt, size of 1GB are initialized in file folder

For throughput: ./exec_name <number of threads (1,2,4)>  <access pattern (RS,WS,RR,WR)> <block size (1,10,100)>(For the block size paramter, 1 means 1MB, 10 means 10MB, 100 means 100MB)
For latency and IOPS: ./exec_name <number of threads (1,2,4,8,16,32,64,128)>  <access pattern (RR,WR)> -l
Example commands:
./mydisk 2 RS 1
./mydisk 16 RR -l

#######################################################################################################
Results of the program contains the execution time and throughput(MB/sec) or latency(ms) and IOPS.
Results will be saves as .txt files in output folder.
