#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

 
int main(){
    char filename[30] = {"book.txt"};
    char number[30];
    int i, stream;
    time_t t1,t2;

    time(&t1);

    if( (stream = open(filename, O_RDWR | O_CREAT, 0744)) == -1){
        printf("\nCan't open file: %s", filename);
        return 1;
    }
    for(i = 0; i < 10000000; i++){
        sprintf(number, "%d\n", rand());
        write(stream, number, sizeof(number));
    }
    close(stream);
    unlink(filename);

    time(&t2);
    printf("Execution duration: %lds\n", t2-t1);

    return 0;
}
