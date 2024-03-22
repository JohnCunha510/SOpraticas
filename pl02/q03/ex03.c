#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){
    int id_proc1, id_proc2, n; 
    pid_t id_wait;
    int status1 = 0;
    int status2 = 0;

    while(status1 < 5 || status2 < 5){  
        if((id_proc1 = fork()) == 0){
            if(status1 > 4){
                 exit(0);
            }
            for(int i = 0; i < 1; i++){
                printf("I am the child 1\n");
            }
            
            exit(0);
        }else if(id_proc1 > 0){
            if(status2 > 4){
                 exit(0);
            }
            if((id_proc2 = fork()) == 0){
                for(int i = 0; i < 1; i++){
                printf("I am the child 2\n");
                }
                
                exit(0);
            }else if(id_proc2 > 0){
                while((id_wait = wait(&n)) == -1);
                if(id_wait == id_proc1){
                    printf("\trelauching child 1\n");
                    status1++;
                }else if(id_wait == id_proc2){
                    printf("\trelauching child 2\n");
                    status2++;
                }else if(id_wait == -1){
                    printf("\trelauching child ERROR\n");
                }else{
                    printf("\trelauching child false\n");
                }
                
            }
            // }else{
            //     perror("fork 2 Fail");
            //     exit(1);
            // }
        }
        // }else{
        //     perror("fork 1 Fail");
        //     exit(1);
        // }
    }
    exit(0);
    return 0;
}