#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

 
int main(){
    char number[4097*30];
    char str[60];
    int i, j, stream, rStream;
    time_t t1,t2;
    int blockSize;

    time(&t1);
    srand(t1);

    if( (rStream = open("results.txt", O_RDWR | O_CREAT, 0744)) == -1){
        printf("\nCan't open file: results.txt");
        return 1;
    }

    for(blockSize = 1; blockSize <= 4096; blockSize = blockSize *2){
        time(&t1);

        if( (stream = open("book.txt", O_RDWR | O_CREAT, 0744)) == -1){
            printf("\nCan't open file: book.txt");
            return 1;
        }
        j = 1;
        for(i = 0; i < 10000000; i++){
            sprintf(&number[j], "%d\n", rand());

            if (j / 30 >= blockSize){
                write(stream, number, j);
                j = 1;
            }
            j = j + 30;
        }
        if (j > 1 ){
            write(stream, number, j);
        }

        close(stream);
        unlink("book.txt");

        time(&t2);
        printf("Block size: %d. Execution duration: %lds\n", blockSize, t2-t1);
        memset(str, 0, 60);
        sprintf(str, "Block size: %d. Execution duration: %lds\n", blockSize, t2-t1);
        write(rStream, str, sizeof(str));
    }

    return 0;
}
