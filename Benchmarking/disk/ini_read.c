#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    FILE *fp_in;
    FILE *fp_in_l;
    int rc;
    //Prepare file to be read
    char *mem = malloc(1000000);
    memset(mem,0,1000000);
    fp_in = fopen("./file/R.txt","w");
    if(fp_in == NULL){
        printf("File is not found\n");
        return 0;
    }
    fp_in_l = fopen("./file/R_l.txt","w");
    if(fp_in_l == NULL){
        printf("File is not found\n");
        return 0;
    }
    
    for(int i=0;i<1e10/1000000+1000;i++){
        //fseek(fp_in,i*1000000,SEEK_SET);
        //long int o = ftell(fp_in);
        //printf("%ld\n",o);
        rc = fwrite(mem,1,1000000,fp_in);
        if(rc != 1000000){
            printf("Write failed\n");
        }
    }
    for(int i=0;i<1e9/1000000+100;i++){
        //fseek(fp_in_l,i*1000000,SEEK_SET);
        //long int o = ftell(fp_in_l);
        //printf("%ld\n",o);
        rc = fwrite(mem,1,1000000,fp_in_l);
        if(rc != 1000000){
            printf("Write failed\n");
        }
    }
    
    fclose(fp_in);
    fclose(fp_in_l);
    
    return 0;
}
