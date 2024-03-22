#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(){
    int id_proc1, id_proc2;
    if(id_proc1 = fork() == 0){
        for(int i = 0; i < 9; i++){
            printf("I am the child 1\n");
        }
    }else if(id_proc1 > 0){
        if(id_proc2 = fork() == 0){
            for(int i = 0; i < 9; i++){
            printf("I am the child 1\n");
        }
        }else if(id_proc2 > 0){


        }else{
            perror("fork 2 Fail");
            exit(1);
        }
    }else{
        perror("fork 1 Fail");
        exit(1);
    }


    printf("\n process_id(pid) = %d \n", getpid());
    return 0;

}