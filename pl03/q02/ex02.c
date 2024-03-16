#include <stdio.h> 
#include <stdlib.h>
#include <signal.h>

/* Global Variables*/
int id_proc = -1;

void sigproc(); 
void quitproc();

main(int argc, char *argv[]) { 
    signal(SIGINT, sigproc); 
    signal(SIGQUIT, quitproc);
    printf("ctrl-c disabled use ctrl- to quit \n\n");

    if((id_proc = fork()) < 0){
        perror("fork Fail");
            exit(1);
    }

    for(;;); /* infinite loop */
}

void sigproc() {
    if( id_proc > 0){
        kill(id_proc, SIGQUIT);
        if((id_proc = fork()) < 0){
        perror("fork Fail");
            exit(1);
        }
    }
}
void quitproc() {
    if( id_proc == 0){
        printf("I’m the child process. Bye bye.\n");
    }else {
        kill(id_proc, SIGQUIT);
        printf("I’m the parent process. End of the program.\n\n");
    }
    exit(0); /* normal exit status */
}