#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

 
int main(){
    char filename[30] = {"book"};
    int i, j;

    for(i = 0; i < 900000; i++){
        sprintf(filename, "test/book%d.txt", i);
        if( open(filename, O_RDWR | O_CREAT, 0744) == -1){
            printf("\nCan't open any more files. Last filename: %s", filename);
            printf("\nCan't open any more files. Last filename: book%d.txt\n", i);
            break;
        }
    }
    for(j = 0; j < i; j++){
        sprintf(filename, "test/book%d.txt", j);
        unlink(filename);
    }

    return 0;
}
