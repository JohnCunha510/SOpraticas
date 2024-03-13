#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>


int main(){

    char option;
    char str[50];
    char command[60] = {};
    char buffer[1024];
    int fd;

    while(1){
        system("clear");

        memset(str, 0, 50);
        memset(command, 0,60);

        printf("MENU\n");
        printf("=================\n");
        printf("1- Edit a file\n");
        printf("2- View a file\n");
        printf("3- List files\n");
        printf("4- Remove file\n");
        printf("s- Quit\n");
        printf("Introduce one option:\n");

        option = getchar();
        while((getchar()) != '\n');

        switch(option){
            case '1':
                printf("File name:\n");
                scanf("%s", str);
                while((getchar()) != '\n');
                fd = open(str, O_RDWR);
                printf("\nPress [Enter] key to continue...\n");
                while((getchar()) != '\n');
                close(fd);
            break;
            case '2':
                printf("File name:\n");
                scanf("%s", str);
                while((getchar()) != '\n');
                fd = open(str, O_RDWR);
                read(fd, buffer, sizeof(buffer));
                printf("Press [Enter] key to continue...\n");
                while((getchar()) != '\n');
                close(fd);
            break;
            case '3':
                system("ls -g");
                printf("\n\nPress [Enter] key to continue...\n");
                while((getchar()) != '\n');
            break;
            case '4':
                printf("File name:\n");
                scanf("%s", str);
                while((getchar()) != '\n');
                strcat(command, "rm ");
                strcat(command, str);
                system(command);
                printf("\nPress [Enter] key to continue...\n");
                while((getchar()) != '\n');
            break;
            case 's':
                printf("EXIT\n");
                exit(0);
            break;
            default:
                printf("Please introduce a valid option\n");
                printf("Press [Enter] key to continue...\n");
                while((getchar()) != '\n');

        }
    }

    return 0;
}
