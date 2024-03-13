#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(){
    printf("Hello World\n");
    pid_t p = fork();
    if(p<0){
        perror("fork Fail");
        exit(1);
    }
    for(int i = 0; i < 99; i++){
        if(p == 0){
            printf("I am the child\n");
        }else{
            printf("I am the father\n");
        }
    }
    printf("\n process_id(pid) = %d \n", getpid());
    return 0;
}
