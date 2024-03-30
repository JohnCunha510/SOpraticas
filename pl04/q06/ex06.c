#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

void print_file_info(const char *filename) {
    struct stat file_stat;
    
    // Get file stats
    if (stat(filename, &file_stat) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    
    // Print file information
    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", file_stat.st_size);
    printf("Permissions: ");
    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
    printf("Last modified: %s", ctime(&file_stat.st_mtime));

    printf("Information directly from stat():\n");
    printf("\tOwner: %d\n", file_stat.st_uid);
    printf("\tGroup: %d\n", file_stat.st_gid);

    printf("Information getpwuid(), getgrgid(), ctime():\n");
    printf("\tOwner: %s\n", getpwuid(file_stat.st_uid)->pw_name);
    printf("\tGroup: %s\n", getgrgid(file_stat.st_gid)->gr_name);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    char *filename = argv[1];
    
    print_file_info(filename);
    
    return EXIT_SUCCESS;
}