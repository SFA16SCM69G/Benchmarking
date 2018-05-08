Program name: MyNETBench-TCP.c and MyNETBench-UDP.c
#######################################################################################################
TCP protocol: MyNETBench-TCP.c
Compile program:
Before compiliing, make sure a folder named "output" is contained in the same directory of MyNETBench-TCP.c
Stardard commands:
Step1: cd directory(the directory contains MyNETBench-TCP.c)
Step2: gcc -pthread -o exec_name MyNETBench-TCP.c
Example commands:
Step1: cd cs553-pa1/network
Step2: gcc -pthread -o mytcpnet MyNETBench-TCP.c
#######################################################################################################
Run program:
Standard commands:
For throughput:
Node 1(server): ./exec_name -s <port number> <number of threads (1,2,4,8)>  <block size (1,32)>(For the block size paramter, 1 means 1KB, 32 means 32KB)
Node 2(client): ./exec_name -c <ip address> <port number> <number of threads (1,2,4,8)>  <block size (1,32)>(For the block size paramter, 1 means 1KB, 32 means 32KB)
For latency:
Node 1(server): ./exec_name -s <port number> <number of threads (1,2,4,8)>  -l
Node 2(client): ./exec_name -c <ip address> <port number> <number of threads (1,2,4,8)> -l

Example commands:
For throughput:
Node 1(server): ./mytcpnet -s 11155 2 32
Node 2(client): ./mytcpnet -c redcompute-1 11155 2 32
For latency:
Node 1(server): ./mytcpnet -s 11155 4 -l
Node 2(client): ./mytcpnet -c redcompute-1 11155 4 -l
#######################################################################################################
UDP protocol: MyNETBench-UDP.c
The same compiling rule and running rule as TCP protocol.
#######################################################################################################
Results of the program contains the execution time and throughput(Mb/sec) or latency(ms).
Results will be saves as .txt files in output folder.
