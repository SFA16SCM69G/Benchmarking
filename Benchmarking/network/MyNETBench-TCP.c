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
//TCP server transfer data by mutiple threads
void *TCP_server_trans(void *newfd){
    //Socket descriptor for all send and recv calls
    int readfd = (int)newfd;
    //For latency
    if(block_size == 1){
        int loop = LATENCY_OPERATION/thread_num;
        //Initialize data
        char data = 0;
        
        for(int i=0;i<loop;i++){
            //Receive data from client
            //Return the number of bytes actually read into block
            rc = recv(readfd, &data, 1, 0);
            if (rc == 0) {
                printf("Connection closed at receive!\n");
                break;
            }
            if (rc < 0) {
                printf("Could not receive package!\n");
                break;
            }
            //Change the value of data before send
            data = 1;
            //Send data to client
            //Return bytes of data actually send out
            rc = send(readfd, &data, 1, 0);
            if (rc < 0) {
                printf("Could not send package!\n");
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

        for(int i=0;i<block_num*LOOP;i++){
            //Receive data from client
            //Return the number of bytes actually read into block
            rc = recv(readfd, block, block_size, 0);
            if (rc == 0) {
                printf("Connection closed at receive!\n");
                break;
            }
            if (rc < 0) {
                printf("Could not receive package!\n");
                break;
            }
            //Change the value in block before send
            memset(block,1,block_size);
            //Send data in block to client
            //Return bytes of data actually send out
            rc = send(readfd, block, block_size, 0);
            if (rc < 0) {
                printf("Could not send package!\n");
            }
        }
        free(block);
    }
}
//Build TCP server
void TCP_server(){
    int sockfd,newfd;
    struct addrinfo hints, *res;
    socklen_t addrlen;
    struct sockaddr_storage clt;
    int yes = 1;
    pthread_t thread[thread_num];
    long threadId;
    void *status;
    //Initialize hints parameter pointer
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
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
    //Listen and wait for client
    rc = listen(sockfd, 10);
    if(rc == -1){
        printf("Could not listen to socket!\n");
        close(sockfd);
        return;
    }
    printf("server listening!\n");
    //Create threads and start routine is TCP_server_trans
    for(threadId=0;threadId<thread_num;threadId++){
        //Accept the client connection request and get the socket descriptor for send and recv
        newfd = accept(sockfd, (struct sockaddr *) &clt, &addrlen);
        if(newfd < 0){
            printf("Could not accept client!\n");
            close(sockfd);
            return;
        }
        pthread_create(&thread[threadId],NULL,TCP_server_trans,newfd);
    }
    //Block until all threads are finished
    for(threadId=0; threadId<thread_num; threadId++) {
        pthread_join(thread[threadId], &status);
    }
    
    freeaddrinfo(res);
    close(sockfd);
}
//TCP client
void *TCP_client(void *threadId){
    int block_num = DATA_SIZE/block_size/thread_num;
    struct addrinfo hints, *res;
    int sockfd;
    long t = (long)threadId;
    
    //Initialize hints parameter pointer
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
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
    //Connect the client to server
    rc = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (rc < 0) {
        printf("Could not connect to server!\n");
        pthread_exit((void*) threadId);
    }
    printf("Client of %ld thread connect success!\n",t);
    //Allocate block size of memory to store data
    char* block = (char *)malloc(block_size);
    //Initialize data in memory
    memset(block, 0, block_size);
    
    for(int i=0;i<block_num*LOOP;i++){
        //Change the value in block before send
        memset(block,1,block_size);
        //Send data in block to server
        //Return bytes of data actually send out
        rc = send(sockfd, block, block_size, 0);
        if (rc < 0) {
            printf("Could not send package!\n");
        }
        //Reset the value in block before recv
        memset(block,0,block_size);
        //Receive data from server
        //Return the number of bytes actually read into block
        rc = recv(sockfd, block, block_size, 0);
        if (rc == 0) {
            printf("Connection closed at receive!\n");
            break;
        }
        if (rc < 0) {
            printf("Could not receive package!\n");
            break;
        }
    }
    free(block);
    
    freeaddrinfo(res);
    pthread_exit((void*) threadId);
}
//TCP client for latency
void *TCP_client_latency(void *threadId){
    int loop = LATENCY_OPERATION/thread_num;
    struct addrinfo hints, *res;
    int sockfd;
    long t = (long)threadId;
    //Initialize hints parameter pointer
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
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
    //Connect the client to server
    rc = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (rc < 0) {
        printf("Could not connect to server!\n");
        pthread_exit((void*) threadId);
    }
    printf("Client of %ld thread connect success!\n",t);
    //Initialize data
    char data = 0;
    
    for(int i=0;i<loop;i++){
        //Change the value of data before send
        data = 1;
        //Send the data to server
        //Return bytes of data actually send out
        rc = send(sockfd,&data,1,0);
        if (rc < 0) {
            printf("Could not send package!\n");
        }
        //Reset the value of data before recv
        data = 0;
        //Receive data from server
        //Return the number of bytes actually read into block
        rc = recv(sockfd,&data,1,0);
        if (rc < 0) {
            printf("Could not receive package!\n");
            break;
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
        TCP_server();
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
            sprintf(filename,"./output/network_tcp_%d_latency.txt",thread_num);
            fp = fopen(filename,"w");
            if(fp == NULL){
                printf("File is not found\n");
                return 0;
            }
            //Create threads and start routine is TCP_client_latency
            gettimeofday(&startTime, NULL);
            for(threadId=0;threadId<thread_num;threadId++){
                pthread_create(&thread[threadId],NULL,TCP_client_latency,(void*)threadId);
            }
            
            //Block until all threads are finished
            for(threadId=0; threadId<thread_num; threadId++) {
                pthread_join(thread[threadId], &status);
            }
            gettimeofday(&endTime,NULL);
            
            //Compute latency
            latency = ((double)(endTime.tv_usec - startTime.tv_usec)/1000 + (double)(endTime.tv_sec - startTime.tv_sec) * 1000)/LATENCY_OPERATION;
            
            rc = fprintf(fp,"%s %d %s %f %s","The latency to transfer 1B data with",thread_num,"threads by TCP protocol for 1 million times is",latency,"ms\n");
            if(rc < 0){
                printf("Write Failed\n");
                return 0;
            }
            fclose(fp);
            
            printf("%s %d %s %f %s","The latency to transfer 1B data with",thread_num,"threads by TCP protocol for 1 million times is",latency,"ms\n");
            return 0;
        }
        
        block_size = atoi(argv[5])*1000;
        sprintf(filename,"./output/network_tcp_%d_%dKB.txt",thread_num,block_size/1000);
        fp = fopen(filename,"w");
        if(fp == NULL){
            printf("File is not found\n");
            return 0;
        }
        double exec_time,throughput;
        
        gettimeofday(&startTime, NULL);
        //Create threads and start routine is TCP_client
        for(threadId=0;threadId<thread_num;threadId++){
            pthread_create(&thread[threadId],NULL,TCP_client,(void*)threadId);
        }
        //Block until all threads are finished
        for(threadId=0; threadId<thread_num; threadId++) {
            pthread_join(thread[threadId], &status);
        }
        gettimeofday(&endTime, NULL);
        
        exec_time = (double)(endTime.tv_usec - startTime.tv_usec)/1000000 +
        (double)(endTime.tv_sec - startTime.tv_sec);
        throughput = LOOP * 1000.0 * 8/exec_time;
        
        rc = fprintf(fp,"%s %d %s %d %s %f %s","Execution time of",thread_num,"threads to transfer 1GB data by TCP protocol by 100 times of",block_size/1000,"KB block size is",exec_time,"seconds\n");
        if(rc < 0){
            printf("Write Failed\n");
            return 0;
        }
        rc = fprintf(fp,"%s %d %s %d %s %f %s","Throughput of",thread_num,"threads to transfer 1GB data by TCP protocol by 100 times of",block_size/1000,"KB block size is",throughput,"Mb/sec\n");
        if(rc < 0){
            printf("Write Failed\n");
            return 0;
        }
        fclose(fp);
        printf("%s %d %s %d %s %f %s","Execution time of",thread_num,"threads to transfer 1GB data by TCP protocol by 100 times of",block_size/1000,"KB block size is",exec_time,"seconds\n");
        printf("%s %d %s %d %s %f %s","Throughput of",thread_num,"threads to transfer 1GB data by TCP protocol by 100 times of",block_size/1000,"KB block size is",throughput,"Mb/sec\n");
        
        return 0;
    }
    else{
        printf("Found failed!\n");
    }
}
