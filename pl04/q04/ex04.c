/*  student database program       */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct vehicle
{
  char name[100];
  char address[100];
  int age;
  int cardNumber;
}record;

int main(void)
{
  int i , choice;
  FILE *fp;
  char sName[100];
  record student;
  int recsize;
  char option;


  do{
    system("clear"); // change to a simpler system call!!

    printf("MENU\n");
    printf("=================\n");
    printf("1- Add new record\n");
    printf("2- Change record\n");
    printf("3- Remove record\n");
    printf("4- Search record by name\n");
    printf("5- Search record by card number\n");
    printf("s- Quit\n");
    printf("Introduce one option:\n");

    if( (fp = fopen("dataStudents.dat", "wb+")) == NULL){
        printf("%s did not open", "dataStudents.dat");
        exit(1);
    }

    option = getchar();
    while((getchar()) != '\n');


    switch(option){
      case '1':
        printf("Student name : ");
        scanf("%[^\n]" , student.name);
        while((getchar()) != '\n');
        printf("Enter address : ");
        scanf("%[^\n]" , student.address);
        while((getchar()) != '\n');
        printf("Enter age : ");
        scanf("%d" , &student.age);
        printf("Enter student card number : ");
        scanf("%d" , &student.cardNumber);

        recsize = sizeof(student);
        fseek(fp , 0 ,SEEK_END);
        fwrite(&student,recsize,1,fp);

        printf("\nPress [Enter] key to continue...\n");
        while((getchar()) != '\n');
        break;
      case '2':

        break;
      case '3':

        break;
      case '4':

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


  }while(1);

  fclose(fp);
}
