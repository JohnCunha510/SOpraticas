#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){
    int id_proc1;

    if((id_proc1 = fork()) == 0){
        printf("I am the child, and my PID is %d and the PID ofmy parent is %d.\n", getpid(), getppid());

    }else if(id_proc1 > 0){
        printf("I am the parent, my PID is %d and the PID of my child is %d.\n", getpid(), id_proc1);
    }else{
        perror("fork 1 Fail");
        exit(1);
    } 
            
    return 0;
}