#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define MEMORY_SIZE 1e9//Size of memory data
#define LATENCY_OPERATION 1e8//Operations for compute latency
//Number of threads
int thread_num;
//Size of block(KB)
int block_size;

//Sequential access pattern
void *RWS(void *threadId){
    //Number of blocks per thread
    int block_num = MEMORY_SIZE/block_size/thread_num;
    //Data to be read and written in one thread
    char *data;
    //Size of data to be read and written in one thread
    int size = MEMORY_SIZE/thread_num;
    //Allocate space for data
    data = (char *) malloc(size);
    //Initialize data
    memset(data,0,size);
    for(int i=0;i<100;i++){
        //Allocate space for memory to read data and be written by data
        char *mem = (char *) malloc(size);
    
        for(int i=0;i<block_num;i++){
            //Sequentially read and write data
            //Each operation read and write block size of data
            memcpy(mem+i*block_size,data+i*block_size,block_size);
        }
        free(mem);
    }
    free(data);
    pthread_exit((void*) threadId);
}

//Random access pattern
void *RWR(void *threadId){
    //Number of blocks per thread
    int block_num = MEMORY_SIZE/block_size/thread_num;
    //Data to be read and written in one thread
    char *data;
    //Size of data to be read and written in one thread
    int size = MEMORY_SIZE/thread_num;
    //Allocate space for data
    data = (char *) malloc(size);
    //Initialize data
    memset(data,0,size);
    for(int i=0;i<100;i++){
        //Allocate space for memory to read data and be written by data
        char *mem = (char *) malloc(size);
    
        for(int i=0;i<block_num;i++){
            //Randomlly read and write data
            //Each operation read and write block size of data
            int offset = rand() % block_num;
            memcpy(mem+(offset*block_size),data+(offset*block_size),block_size);
        }
        free(mem);
    }
    free(data);
    pthread_exit((void*) threadId);
}

//Sequential access pattern for latency
void *RWS_latency(void *threadId){
    //Allocate and initialize data
    char data=0;
    //Number of operations to read and write data in one thread
    int loop = LATENCY_OPERATION/thread_num;
    //Allocate space for memory to read data and be written by data
    char *mem = (char *) malloc(loop);
    
    for(int i=0;i<loop;i++){
        //Sequentially read and write 1 byte data
        memcpy(mem+i,&data,1);
    }
    
    free(mem);
    pthread_exit((void*) threadId);
}

//Random access pattern for latency
void *RWR_latency(void *threadId){
    //Allocate and initialize data
    char data=0;
    //Number of operations to read and write data in one thread
    int loop = LATENCY_OPERATION/thread_num;
    //Allocate space for memory to read data and be written by data
    char *mem = (char *) malloc(loop);
    
    for(int i=0;i<loop;i++){
        //Randomlly read and write 1 byte data
        int offset = rand() % loop;
        memcpy(mem+offset,&data,1);
    }
    
    free(mem);
    pthread_exit((void*) threadId);
}

int main(int argc,char *argv[]) {
    FILE *fp;
    char *filename[50];
    int rc;
    
    thread_num = atoi(argv[1]);
    char *access_pattern = argv[2];
    char *flag = argv[3];
    
    pthread_t thread[thread_num];
    struct timeval startTime, endTime;
    long threadId;
    void *status;
    
    //Compute latency
    //Default block size is 1 byte
    if(strcmp(flag,"-l") == 0){
        double latency;
        sprintf(filename,"./output/memory_%d_%s_latency.txt",thread_num,access_pattern);
        fp = fopen(filename,"w");
        if(fp == NULL){
            printf("File is not found\n");
            return 0;
        }
        
        gettimeofday(&startTime,NULL);
        //Create threads and start routine is determined by access pattern
        for(threadId=0;threadId<thread_num;threadId++){
            if(strcmp(access_pattern,"RWS") == 0)
                pthread_create(&thread[threadId],NULL,RWS_latency,(void*)threadId);
            else if(strcmp(access_pattern,"RWR") == 0)
                pthread_create(&thread[threadId],NULL,RWR_latency,(void*)threadId);
            else{
                printf("Not a correct access pattern\n");
                return 0;
            }
        }
        
        //Block until all threads are finished
        for(threadId=0; threadId<thread_num; threadId++) {
            pthread_join(thread[threadId], &status);
        }
        gettimeofday(&endTime,NULL);
        //Compute latency
        latency = ((double)(endTime.tv_usec - startTime.tv_usec) + (double)(endTime.tv_sec - startTime.tv_sec) * 1000000)/LATENCY_OPERATION;
        
        rc = fprintf(fp,"%s %s %s %f %s","The latency of reading 1 Byte data by",access_pattern,"100 million times is",latency,"micro seconds\n");
        if(rc < 0){
            printf("Write Failed\n");
            return 0;
        }
        fclose(fp);
        printf("%s %s %s %f %s","The latency of reading 1 Byte data by",access_pattern,"100 million times is",latency,"micro seconds\n");
        
        return 0;
    }
    
    block_size = atoi(argv[3])*1000;
    sprintf(filename,"./output/memory_%d_%s_%dKB.txt",thread_num,access_pattern,block_size/1000);
    fp = fopen(filename,"w");
    if(fp == NULL){
        printf("File is not found\n");
        return 0;
    }
    double exec_time,throughput;
    
    gettimeofday(&startTime, NULL);
    //Create threads and start routine is determined by access pattern
    for(threadId=0;threadId<thread_num;threadId++){
        if(strcmp(access_pattern,"RWS") == 0){
            pthread_create(&thread[threadId],NULL,RWS,(void*)threadId);
        }
        else if(strcmp(access_pattern,"RWR") == 0){
            pthread_create(&thread[threadId],NULL,RWR,(void*)threadId);
        }
        else{
            printf("Not a correct access pattern\n");
            return 0;
        }
    }
    //Block until all threads are finished
    for(threadId=0; threadId<thread_num; threadId++) {
        pthread_join(thread[threadId], &status);
    }
    gettimeofday(&endTime, NULL);
    
    exec_time = (double)(endTime.tv_usec - startTime.tv_usec)/1000000 +
    (double)(endTime.tv_sec - startTime.tv_sec);
    throughput = 100.0/exec_time;
    
    rc = fprintf(fp,"%s %d %s %s %s %d%s %f %s","Execution time of",thread_num,"threads,",access_pattern,"access pattern to read and write 100GB data by",block_size/1000,"KB block size is",exec_time,"seconds\n");
    if(rc < 0){
        printf("Write Failed\n");
        return 0;
    }
    rc = fprintf(fp,"%s %d %s %s %s %d%s %f %s","Throughput of",thread_num,"threads",access_pattern,"access pattern to read and write 100GB data by",block_size/1000,"KB block size is",throughput,"GB/sec\n");
    if(rc < 0){
        printf("Write Failed\n");
        return 0;
    }
    fclose(fp);
    printf("%s %d %s %s %s %d%s %f %s","Execution time of",thread_num,"threads,",access_pattern,"access pattern to read and write 100GB data by",block_size/1000,"KB block size is",exec_time,"seconds\n");
    printf("%s %d %s %s %s %d%s %f %s","Throughput of",thread_num,"threads",access_pattern,"access pattern to read and write 100GB data by",block_size/1000,"KB block size is",throughput,"GB/sec\n");
    
    return 0;
}
