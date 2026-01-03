#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(void) {
    int fileDescriptor[2];
    if (pipe(fileDescriptor) == -1) {
        perror("pipe");
        return 1;
    }
    pid_t child;
    if ((child = fork()) == -1) {
        perror("fork");
        return 1;
    }
    if (child == 0) {

    } else {

    }
    return 0;
}