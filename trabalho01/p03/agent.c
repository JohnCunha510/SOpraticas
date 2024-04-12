#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

void catch_HUP(){
    char character;
    // Open file pointer for text.in
    FILE *text_file = fopen("text.in", "r");
    if (text_file == NULL) {
        perror("Error Readind text.in file");
        exit(1);
    }
    // Read text.in to terminal
    while((character = fgetc(text_file)) != EOF){
        printf("%c", character);
    }
    fclose(text_file);
}
void catch_TERM(){
    printf("Process terminating...\n");
    unlink("agent.pid");
    exit(0);
}

int main(int argc, char *argv[]) {
    char character;

    // Create a file pointer for the .pid file
    FILE *pid_file = fopen("agent.pid", "w");
    if (pid_file == NULL) {
        perror("Error Creating .pid file");
        exit(1);
    }

    // Get the process ID of the current process
    pid_t pid = getpid();

    // Write the process ID to the .pid file
    fprintf(pid_file, "%d\n", pid);
    fclose(pid_file);

    // Open file pointer for text.in
    FILE *text_file = fopen("text.in", "r");
    if (text_file == NULL) {
        perror("Error Readind text.in file");
        exit(1);
    }
    // Read text.in to terminal
    while((character = fgetc(text_file)) != EOF){
        printf("%c", character);
    }
    fclose(text_file);

    // Signal Handlers
    signal(SIGHUP, catch_HUP);
    signal(SIGTERM, catch_TERM);

    for( ; ; )
        pause();

    return 0;
}