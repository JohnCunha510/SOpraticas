#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <signal.h>

FILE *input_bin, *output;
sem_t *semaphore;
int shm_fd;
double *shared_memory;

void catch_SIGNAL(){
    sem_close(semaphore); // close semaphore
    munmap(shared_memory, sizeof(double)); // unmap shared memory
    close(shm_fd); // close shared memory
    fclose(input_bin);// Close input.bin file
    fclose(output); // close output.asc

    exit(1);
}

int main(int argc, char *argv[]) {

    double buff_input[1024];
    int i = 0; int n_numbers = 0;

    // Signal Handlers
    signal(SIGHUP, catch_SIGNAL);
    signal(SIGTERM, catch_SIGNAL);
    signal(SIGINT, catch_SIGNAL);
    signal(SIGQUIT, catch_SIGNAL);
    signal(SIGKILL, catch_SIGNAL);

    // Open a shared memory
    int shm_fd = shm_open("my_shared_memory.bin", O_RDONLY, 0666);
    if(shm_fd == -1){
        perror("my_shared_memory.bin did not open");
        return 1;
    }
    // map shared memory
    double *shared_memory = (double *)mmap(NULL, sizeof(double), PROT_READ , MAP_SHARED, shm_fd, 0);
    if(shared_memory == (double *) -1){
        perror("shared_memory did not map");
        return 1;
    }

    // open the semaphore
    if((semaphore = sem_open("my_semaphore", O_RDWR)) == SEM_FAILED){
        perror("my_semaphore did not open");
        return 1;
    }
    // open input.bin for writing
    if ((input_bin = fopen("input.bin", "w")) == NULL) {
        perror("input.bin did not create");
        exit(1);
    }

    // send semaphore and read shared memory to input.bin file
    // 1 double at a time
    // if shared memory == DBL_MIN, stop reading shared memory
    // and send 1 last semaphore so client process can close and unlink the smaphore
    sem_post(semaphore);
    usleep(100);
    while (*shared_memory != DBL_MIN) {
        n_numbers += 1;
        fwrite(shared_memory, sizeof(double), 1, input_bin);
        //printf("Value read from shared memory: %f\n", *shared_memory);
        sem_post(semaphore); // Signal client that it can write the next value
        usleep(100);
    }
    sem_post(semaphore);
    sem_close(semaphore);

    //printf("%d\n", n_numbers);

    // unmap shared memory
    if(munmap(shared_memory, sizeof(double)) == -1){
    perror("shared_memory did not unmap");
        return 1;
    }
    close(shm_fd); // close shared memory

    fclose(input_bin); // close input.bin file

    // reopen input.bin for reading
    if ((input_bin = fopen("input.bin", "r")) == NULL) {
        perror("input.bin did not create");
        exit(1);
    }
    // read "n_numbers" from input.bin to array "buff_input"
    fread(buff_input, sizeof(double), n_numbers, input_bin);

    // open output.asc for writing
    if ((output = fopen("output.asc", "w")) == NULL) {
        perror("output.asc did not create");
        exit(1);
    }
    // print all doubles * 4.0 to output.asc
    for(i = 0; i < n_numbers; i++){
        buff_input[i] = buff_input[i] * 4.0;
        fprintf(output,"%f\n", buff_input[i]);
        //printf("%f\n", buff_input[i]);
    }
    fclose(output); // close output.asc
    
    return 0;
}

