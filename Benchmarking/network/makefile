network : MyNETBench-TCP.o MyNETBench-UDP.o
	cc -o mytcpnet MyNETBench-TCP.o -pthread
	cc -o myudpnet MyNETBench-UDP.o -pthread
   MyNETBench-TCP.o : MyNETBench-TCP.c
	cc -c MyNETBench-TCP.c -lm
   MyNETBench-UDP.o : MyNETBench-UDP.c
	cc -c MyNETBench-UDP.c -lm

clean:
	rm -f *.o mytcpnet myudpnet


