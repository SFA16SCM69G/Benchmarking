#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define DISK_SIZE 1e10//Size of disk data
#define LATENCY_OPERATION 10e6//Operations for compute latency
//Number of threads
int thread_num;
//Size of block(MB)
int block_size;
//File to be operated
FILE *fp;
//Return value of file operations
int rc;
//Count for failed read
int count=0;

//Read with sequential access pattern
void *RS(void *threadId){
    //Number of blocks per thread
    int block_num = DISK_SIZE/block_size/thread_num;
    //Open the file to be read
    fp = fopen("./file/R.txt","r");
    if(fp == NULL){
        printf("Open failed\n");
        pthread_exit((void*) threadId);
    }
    
    for(int i=0;i<block_num;i++){
        //Allocate a block size of memory to read data
        char *mem = (char *)malloc(block_size);
        rc = fread(mem,1,block_size,fp);
        //last read do not need to flush
        if(i==block_num-1)
            continue;
        //flush the cache
        int fd = fileno(fp);
        int ret = fsync(fd);
        free(mem);
    }
    
    pthread_exit((void*) threadId);
}
//Write with sequential access pattern
void *WS(void *threadId){
    //long tid;
    //tid = (long)threadId;
    //Number of blocks per thread
    int block_num = DISK_SIZE/block_size/thread_num;
    //Open the file to be written
    fp = fopen("./file/W.txt","w");
    if(fp == NULL){
        printf("Open failed\n");
        pthread_exit((void*) threadId);
    }
    
    //fseek(fp,(DISK_SIZE/THREAD_NUM)*tid,SEEK_SET);
    //Allocate a block size of memory to write data
    char *mem = (char *)malloc(block_size);
    //Initialize data to write
    memset(mem,0, block_size);
    
    for(int i=0;i<block_num;i++){
        //Write a block size of data from memory to file
        rc = fwrite(mem,1,block_size,fp);
        //last write do not need to flush
        if(i==block_num-1)
            continue;
        //flush the cache
        int fd = fileno(fp);
        int ret = fsync(fd);
    }
    
    free(mem);
    pthread_exit((void*) threadId);
}
//Read with random access pattern
void *RR(void *threadId){
    //Number of blocks per thread
    int block_num = DISK_SIZE/block_size/thread_num;
    //Open the file to be read
    fp = fopen("./file/R.txt","r");
    if(fp == NULL){
        printf("Open failed\n");
        pthread_exit((void*) threadId);
    }
    
    for(int i=0;i<block_num;i++){
        //Allocate a block size of memory to read data
        char *mem = (char *)malloc(block_size);
        //Get offset randomlly
        long int offset = rand() % block_num;
        //Point to the offset position
        if(offset*block_num <= 2000000000){
            fseek(fp,offset*block_size,SEEK_SET);
        }
        else if(offset*block_num <= 4000000000){
            fseek(fp,2000000000,SEEK_SET);
            fseek(fp,offset*block_size-2000000000,SEEK_CUR);
        }
        else if(offset*block_num <= 6000000000){
            fseek(fp,2000000000,SEEK_SET);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,offset*block_size-4000000000,SEEK_CUR);
        }
        else if(offset*block_num <= 8000000000){
            fseek(fp,2000000000,SEEK_SET);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,offset*block_size-6000000000,SEEK_CUR);
        }
        else{
            fseek(fp,2000000000,SEEK_SET);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,offset*block_size-8000000000,SEEK_CUR);
        }
        
        //Read a block size of data from file to memory
        rc = fread(mem,1,block_size,fp);
        
        //last read do not need to flush
        if(i==block_num-1)
            continue;
        //flush the cache
        int fd = fileno(fp);
        int ret = fsync(fd);
        free(mem);
    }
    
    pthread_exit((void*) threadId);
}
//Write with random access pattern
void *WR(void *threadId){
    //Number of blocks per thread
    int block_num = DISK_SIZE/block_size/thread_num;
    //Open the file to be written
    fp = fopen("./file/W.txt","w");
    if(fp == NULL){
        printf("Open failed\n");
        pthread_exit((void*) threadId);
    }
    
    //Allocate a block size of memory to write data
    char *mem = (char *)malloc(block_size);
    //Initialize data to write
    memset(mem,0, block_size);
    
    for(int i=0;i<block_num;i++){
        //Get offset randomlly
        long int offset = rand() % block_num;
        //Point to the offset position
        if(offset*block_num <= 2000000000){
            fseek(fp,offset*block_size,SEEK_SET);
        }
        else if(offset*block_num <= 4000000000){
            fseek(fp,2000000000,SEEK_SET);
            fseek(fp,offset*block_size-2000000000,SEEK_CUR);
        }
        else if(offset*block_num <= 6000000000){
            fseek(fp,2000000000,SEEK_SET);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,offset*block_size-4000000000,SEEK_CUR);
        }
        else if(offset*block_num <= 8000000000){
            fseek(fp,2000000000,SEEK_SET);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,offset*block_size-6000000000,SEEK_CUR);
        }
        else{
            fseek(fp,2000000000,SEEK_SET);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,2000000000,SEEK_CUR);
            fseek(fp,offset*block_size-8000000000,SEEK_CUR);
        }
        //Write a block size of data from memory to file
        rc = fwrite(mem,1,block_size,fp);
        //last write do not need to flush
        if(i==block_num-1)
            continue;
        //flush the cache
        int fd = fileno(fp);
        int ret = fsync(fd);
    }
    
    free(mem);
    pthread_exit((void*) threadId);
}
//Read with random access pattern for latency
void *RR_latency(void *threadId){
    //Number of operations
    int loop = LATENCY_OPERATION/thread_num;
    //Open the file to be read
    fp = fopen("./file/R_l.txt","r");
    if(fp == NULL){
        printf("Open failed\n");
    }
    
    for(int i=0;i<loop;i++){
        //Allocate a 1KB memory to read data
        char *mem = (char *)malloc(1000);
        //Get offset randomlly
        int offset = rand() % loop;
        //Point to the offset position
        fseek(fp,offset*1000,SEEK_SET);
        //Read a 1KB data from file to memory
        rc = fread(mem,1,1000,fp);
        if(rc != 1000){
            count ++;
        }
        free(mem);
    }
    
    pthread_exit((void*) threadId);
}
//Write with random access pattern for latency
void *WR_latency(void *threadId){
    //Number of operations
    int loop = LATENCY_OPERATION/thread_num;
    //Open the file to be written
    fp = fopen("./file/W_l.txt","w");
    if(fp == NULL){
        printf("Open failed\n");
    }
    
    //Allocate 1KB memory to write data
    char *mem = (char *)malloc(1000);
    //Initialize data to write
    memset(mem,0,1000);
    
    for(int i=0;i<loop;i++){
        //Get offset randomlly
        int offset = rand() % loop;
        //Point to the offset position
        rc = fseek(fp,offset*1000,SEEK_SET);
        //Write a 1KB data from memory to file
        rc = fwrite(mem,1,1000,fp);
        if(rc != 1000){
            count ++;
        }
    }
    
    free(mem);
    pthread_exit((void*) threadId);
}

int main(int argc,char *argv[]) {
    FILE *fp_out;
    char *filename[50];
    
    thread_num = atoi(argv[1]);
    char *access_pattern = argv[2];
    char *flag = argv[3];
    
    pthread_t thread[thread_num];
    struct timeval startTime, endTime;
    long threadId;
    void *status;
    
    //Compute latency and IOPS
    //Default block size is 1KB byte
    if(strcmp(flag,"-l") == 0){
        double latency,iops;
        sprintf(filename,"./output/disk_%d_%s_latency.txt",thread_num,access_pattern);
        fp_out = fopen(filename,"w");
        if(fp_out == NULL){
            printf("File is not found\n");
            return 0;
        }
        
        gettimeofday(&startTime,NULL);
        //Create threads and start routine is determined by access pattern
        for(threadId=0;threadId<thread_num;threadId++){
            if(strcmp(access_pattern,"RR") == 0)
                pthread_create(&thread[threadId],NULL,RR_latency,(void*)threadId);
            else if(strcmp(access_pattern,"WR") == 0)
                pthread_create(&thread[threadId],NULL,WR_latency,(void*)threadId);
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
        latency = ((double)(endTime.tv_usec - startTime.tv_usec)/1000 + (double)(endTime.tv_sec - startTime.tv_sec) * 1000)/(LATENCY_OPERATION-count);
        iops = 1000.0/latency;
        
        rc = fprintf(fp_out,"%s %s %s %d %s %f %s","The latency of reading 1KB data by",access_pattern,"with",thread_num,"threads 1 million times is",latency,"ms\n");
        if(rc < 0){
            printf("Write Failed\n");
            return 0;
        }
        rc = fprintf(fp_out,"%s %d %s %f\n","The IOPS with",thread_num,"threads is",iops);
        if(rc < 0){
            printf("Write Failed\n");
            return 0;
        }
        fclose(fp_out);
        printf("%s %s %s %d %s %f %s","The latency of reading 1KB data by",access_pattern,"with",thread_num,"threads 1 million times is",latency,"ms\n");
        printf("%s %d %s %f\n","The IOPS with",thread_num,"threads is",iops);
        
        return 0;
    }
    
    block_size = atoi(argv[3])*1000000;
    sprintf(filename,"./output/disk_%d_%s_%dMB.txt",thread_num,access_pattern,block_size/1000000);
    fp_out = fopen(filename,"w");
    if(fp_out == NULL){
        printf("File is not found\n");
        return 0;
    }
    double exec_time,throughput;
    
    gettimeofday(&startTime, NULL);
    //Create threads and start routine is determined by access pattern
    for(threadId=0;threadId<thread_num;threadId++){
        if(strcmp(access_pattern,"RS") == 0){
            pthread_create(&thread[threadId],NULL,RS,(void*)threadId);
        }
        else if(strcmp(access_pattern,"WS") == 0){
            pthread_create(&thread[threadId],NULL,WS,(void*)threadId);
        }
        else if(strcmp(access_pattern,"RR") == 0){
            pthread_create(&thread[threadId],NULL,RR,(void*)threadId);
        }
        else if(strcmp(access_pattern,"WR") == 0){
            pthread_create(&thread[threadId],NULL,WR,(void*)threadId);
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
    throughput = 10000.0/exec_time;
    
    rc = fprintf(fp_out,"%s %d %s %s %s %d%s %f %s","Execution time of",thread_num,"threads,",access_pattern,"access pattern to read and write 10GB data by",block_size/1000000,"MB block size is",exec_time,"seconds\n");
    if(rc < 0){
        printf("Write Failed\n");
        return 0;
    }
    rc = fprintf(fp_out,"%s %d %s %s %s %d%s %f %s","Throughput of",thread_num,"threads",access_pattern,"access pattern to read and write 10GB data by",block_size/1000000,"MB block size is",throughput,"MB/sec\n");
    if(rc < 0){
        printf("Write Failed\n");
        return 0;
    }
    fclose(fp_out);
    printf("%s %d %s %s %s %d%s %f %s","Execution time of",thread_num,"threads,",access_pattern,"access pattern to read and write 10GB data by",block_size/1000000,"MB block size is",exec_time,"seconds\n");
    printf("%s %d %s %s %s %d%s %f %s","Throughput of",thread_num,"threads",access_pattern,"access pattern to read and write 10GB data by",block_size/1000000,"MB block size is",throughput,"MB/sec\n");
    
    return 0;
}
