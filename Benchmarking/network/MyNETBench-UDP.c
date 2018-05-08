#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
//The total size of data will be transferred
#define DATA_SIZE 1e9
#define LATENCY_OPERATION 1e6//Operations for compute latency
//Number of times
#define LOOP 100
//Size of block(KB)
int block_size;
//Number of threads
int thread_num;
//Return value of file operations
int rc;
//Ip address
char *ip;
//Port number
char *port;
//Parameter for sendto and recvfrom
struct sockaddr_storage server_addr;
//UDP server transfer data by mutiple threads
void *UDP_server_trans(void *sockfd){
    socklen_t addr_len;
    //Socket descriptor
    int readfd = (int)sockfd;
    //For latency
    if(block_size == 1){
        int loop = LATENCY_OPERATION/thread_num;
        //Initialize data
        char data = 0;
        
        for(int i=0;i<loop-1;i++){
            //Receive data from client
            //Return the number of bytes actually read into block
            rc = recvfrom(readfd, &data, 1, 0, (struct sockaddr *)&server_addr,&addr_len);
            while(rc < 0){
                rc = recvfrom(readfd, &data, 1, 0, (struct sockaddr *)&server_addr,&addr_len);
            }
            if (rc == 0) {
                printf("Connection closed at receive!\n");
                break;
            }
        }
    }
    //For throughput
    else{
        //Number of block of each thread
        int block_num = DATA_SIZE/block_size/thread_num;
        //Allocate block size of memory to store data
        char* block = (char *)malloc(block_size);
        //Initialize data in memory
        memset(block, 0, block_size);
        for(int i=0;i<block_num*LOOP-1;i++){
            //Receive data from client
            //Return the number of bytes actually read into block
            rc = recvfrom(readfd, block, block_size, 0, (struct sockaddr *)&server_addr,&addr_len);
            while(rc < 0){
                rc = recvfrom(readfd, block, block_size, 0, (struct sockaddr *)&server_addr,&addr_len);
            }
            if (rc == 0) {
                printf("Connection closed at receive!\n");
                break;
            }
        }
        free(block);
    }
}
//Build UDP server
void UDP_server(){
    int sockfd;
    struct addrinfo hints, *res;
    socklen_t addr_len;
    struct sockaddr_storage clt;
    int yes=1;
    pthread_t thread[thread_num];
    long threadId;
    void *status;
    //Initialize hints parameter pointer
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;//Use my IP
    //Get address information and res pass to other socket functions
    rc = getaddrinfo(NULL, port, &hints, &res);
    if(rc != 0){
        printf("Could not get address information!\n");
        return;
    }
    //Get the socket descriptor
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd < 0){
        printf("Could not create socket!\n");
        return;
    }
    //Reset an exist port
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        printf("Port is not avaiable!\n");
        return;
    }
    //Associate the socket with port on local machine
    rc = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if(rc < 0){
        printf("Could not bind socket!\n");
        close(sockfd);
        return;
    }
    
    addr_len = sizeof(clt);
    //Allocate block size of memory to store data
    char* block = (char *)malloc(block_size);
    printf("Server is waiting to receive data\n");
    //Create threads and start routine is UDP_server_trans
    for(threadId=0;threadId<thread_num;threadId++){
        //Constant recvfrom until a block size of data is received successfully
        rc = recvfrom(sockfd, block, block_size, 0, (struct sockaddr *)&server_addr,&addr_len);
        while(rc < 0){
            rc = recvfrom(sockfd, block, block_size, 0, (struct sockaddr *)&server_addr,&addr_len);
        }
        printf("First data of %ld thread server is received\n",threadId);
        pthread_create(&thread[threadId],NULL,UDP_server_trans,sockfd);
    }
    //Block until all threads are finished
    for(threadId=0; threadId<thread_num; threadId++) {
        pthread_join(thread[threadId], &status);
    }
    
    free(block);
    freeaddrinfo(res);
    close(sockfd);
}
//UDP client
void *UDP_client(void *threadId){
    int block_num = DATA_SIZE/block_size/thread_num;
    struct addrinfo hints, *res;
    int sockfd;
    long t = (long)threadId;
    //Initialize hints parameter pointer
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    //Get address information and res pass to other socket functions
    rc = getaddrinfo(ip, port, &hints, &res);
    if (rc != 0) {
        printf("Could not get address information!\n");
        pthread_exit((void*) threadId);
    }
    //Get the socket descriptor
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        printf("Could not create socket!\n");
        pthread_exit((void*) threadId);
    }
    //Allocate block size of memory to store data
    char* block = (char *)malloc(block_size);
    //Initialize data in memory
    memset(block, 1, block_size);
    //Send the first data
    rc = sendto(sockfd,block,block_size,0,res->ai_addr,res->ai_addrlen);
    if (rc < 0) {
        printf("Could not send package!\n");
    }
    printf("First data of %ld client is sent\n",t);
    
    for(int i=0;i<block_num*LOOP-1;i++){
        //Send data in block to server
        //Return bytes of data actually send out
        rc = sendto(sockfd,block,block_size,0,res->ai_addr,res->ai_addrlen);
        if (rc < 0) {
            printf("Could not send package!\n");
        }
    }
    free(block);
    
    freeaddrinfo(res);
    pthread_exit((void*) threadId);
}
//UDP client for latency
void *UDP_client_latency(void *threadId){
    int loop = LATENCY_OPERATION/thread_num;
    struct addrinfo hints, *res;
    int sockfd;
    long t = (long)threadId;
    //Initialize hints parameter pointer
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    //Get address information and res pass to other socket functions
    rc = getaddrinfo(ip, port, &hints, &res);
    if (rc != 0) {
        printf("Could not get address information!\n");
        pthread_exit((void*) threadId);
    }
    //Get the socket descriptor
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        printf("Could not create socket!\n");
        pthread_exit((void*) threadId);
    }
    //Initialize data
    char data = 0;
    //Send the first data
    rc = sendto(sockfd,&data,1,0,res->ai_addr,res->ai_addrlen);
    if (rc < 0) {
        printf("Could not send package!\n");
    }
    printf("First data of %ld client is sent\n",t);
    
    for(int i=0;i<loop-1;i++){
        //Change the value of data before send
        data = 1;
        //Send the data to server
        //Return bytes of data actually send out
        rc = sendto(sockfd,&data,1,0,res->ai_addr,res->ai_addrlen);
        if (rc < 0) {
            printf("Could not send package!\n");
        }
    }
    
    freeaddrinfo(res);
    pthread_exit((void*) threadId);
}

int main(int argc,char *argv[]) {
    //Build a server
    if(strcmp(argv[1],"-s") == 0){
        port = argv[2];
        thread_num = atoi(argv[3]);
        char *flag = argv[4];
        if(strcmp(flag,"-l") == 0){
            block_size = 1;
        }
        else{
            block_size = atoi(argv[4])*1000;
        }
        UDP_server();
    }
    //Build clients with number of thread_num and connect to server
    else if(strcmp(argv[1],"-c") == 0){
        FILE *fp;
        char *filename[50];
        
        ip = argv[2];
        port = argv[3];
        thread_num = atoi(argv[4]);
        char *flag = argv[5];
        
        pthread_t thread[thread_num];
        long threadId;
        void *status;
        struct timeval startTime, endTime;
        
        //Compute latency
        //Default block size is 1B byte
        if(strcmp(flag,"-l") == 0){
            double latency;
            sprintf(filename,"./output/network_udp_%d_latency.txt",thread_num);
            fp = fopen(filename,"w");
            if(fp == NULL){
                printf("File is not found\n");
                return 0;
            }
            //Create threads and start routine is UDP_client_latency
            gettimeofday(&startTime, NULL);
            for(threadId=0;threadId<thread_num;threadId++){
                pthread_create(&thread[threadId],NULL,UDP_client_latency,(void*)threadId);
            }
            
            //Block until all threads are finished
            for(threadId=0; threadId<thread_num; threadId++) {
                pthread_join(thread[threadId], &status);
            }
            gettimeofday(&endTime,NULL);
            
            //Compute latency
            latency = ((double)(endTime.tv_usec - startTime.tv_usec)/1000 + (double)(endTime.tv_sec - startTime.tv_sec) * 1000)/LATENCY_OPERATION;
            
            rc = fprintf(fp,"%s %d %s %f %s","The latency to transfer 1B data with",thread_num,"threads by UDP protocol for 1 million times is",latency,"ms\n");
            if(rc < 0){
                printf("Write Failed\n");
                return 0;
            }
            fclose(fp);
            
            printf("%s %d %s %f %s","The latency to transfer 1B data with",thread_num,"threads by UDP protocol for 1 million times is",latency,"ms\n");
            return 0;
        }
        
        block_size = atoi(argv[5])*1000;
        sprintf(filename,"./output/network_udp_%d_%dKB.txt",thread_num,block_size/1000);
        fp = fopen(filename,"w");
        if(fp == NULL){
            printf("File is not found\n");
            return 0;
        }
        double exec_time,throughput;
        
        gettimeofday(&startTime, NULL);
        //Create threads and start routine is UDP_client
        for(threadId=0;threadId<thread_num;threadId++){
            pthread_create(&thread[threadId],NULL,UDP_client,(void*)threadId);
        }
        //Block until all threads are finished
        for(threadId=0; threadId<thread_num; threadId++) {
            pthread_join(thread[threadId], &status);
        }
        gettimeofday(&endTime, NULL);
        
        exec_time = (double)(endTime.tv_usec - startTime.tv_usec)/1000000 +
        (double)(endTime.tv_sec - startTime.tv_sec);
        throughput = LOOP * 1000.0 * 8/exec_time;
        
        rc = fprintf(fp,"%s %d %s %d %s %f %s","Execution time of",thread_num,"threads to transfer 1GB data by UDP protocol by 100 times of",block_size/1000,"KB block size is",exec_time,"seconds\n");
        if(rc < 0){
            printf("Write Failed\n");
            return 0;
        }
        rc = fprintf(fp,"%s %d %s %d %s %f %s","Throughput of",thread_num,"threads to transfer 1GB data by UDP protocol by 100 times of",block_size/1000,"KB block size is",throughput,"Mb/sec\n");
        if(rc < 0){
            printf("Write Failed\n");
            return 0;
        }
        fclose(fp);
        printf("%s %d %s %d %s %f %s","Execution time of",thread_num,"threads to transfer 1GB data by UDP protocol by 100 times of",block_size/1000,"KB block size is",exec_time,"seconds\n");
        printf("%s %d %s %d %s %f %s","Throughput of",thread_num,"threads to transfer 1GB data by UDP protocol by 100 times of",block_size/1000,"KB block size is",throughput,"Mb/sec\n");
        
        return 0;
    }
    else{
        printf("Found failed!\n");
    }
}
