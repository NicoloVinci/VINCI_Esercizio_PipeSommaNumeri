#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

int main(void) {
    int fileDescriptor[2];
    if (pipe(fileDescriptor) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    int fileDescriptor1[2];
    if (pipe(fileDescriptor1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t child;
    if ((child = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child == 0) {
        close(fileDescriptor[1]);
        close(fileDescriptor1[0]);
        ssize_t n;
        int num1, num2, sum = 0, i = 0;
        if (read(fileDescriptor[0], &num1, sizeof(int)) != sizeof(int)) {
            perror("read from father");
            exit(EXIT_FAILURE);
        }
        i++;
        if (read(fileDescriptor[0], &num2, sizeof(int)) != sizeof(int)) {
            perror("read from father");
            exit(EXIT_FAILURE);
        }
        i++;
        if (i != 2) {
            printf("Ricevuti numeri non validi.\n");
            close(fileDescriptor[0]);
			close(fileDescriptor1[1]);
            exit(EXIT_FAILURE);
        }
        close(fileDescriptor[0]);
        sum = num1 + num2;
        if (write(fileDescriptor1[1], &sum, sizeof(int)) != sizeof(int)) {
            perror("write to father");
            exit(EXIT_FAILURE);
        }
        close(fileDescriptor1[1]);
        exit(EXIT_SUCCESS);
    } else {
        close(fileDescriptor1[1]);
        close(fileDescriptor[0]);
        printf("Inserisci due numeri interi:\n");
        int a, b;
        for (int i = 0; i < 2; i++) {
            char buf[64];
            char *endptr;
            if (!fgets(buf, sizeof(buf), stdin)) {
                printf("Numero non valido, inserisci un numero intero.\n");
                i--;
                continue;
            }
            if (buf[0] == '\n') {
                printf("Numero non valido, inserisci un numero intero.\n");
                i--;
                continue;
            }
            errno = 0;
            long val = strtol(buf, &endptr, 10);
            if (endptr == buf || errno == ERANGE) {
                printf("Numero non valido, inserisci un numero intero.\n");
                i--;
                continue;
            }
            while (*endptr == ' ' || *endptr == '\t') {
                endptr++;
            }
            if (*endptr != '\n' && *endptr != '\0') {
                printf("Numero non valido, inserisci un numero intero.\n");
                i--;
                continue;
            }
            if (val < INT_MIN || val > INT_MAX) {
                printf("Numero non valido, inserisci un numero intero.\n");
                i--;
                continue;
            }
            if (i == 0) {
                a = (int)val;
            } else {
                b = (int)val;
            }
        }
        write(fileDescriptor[1], &a, sizeof(int));
        write(fileDescriptor[1], &b, sizeof(int));
        close(fileDescriptor[1]);
        wait(NULL);
        int result;
        if (read(fileDescriptor1[0], &result, sizeof(int)) != sizeof(int)) {
            perror("read from child");
            exit(EXIT_FAILURE);
        }
        close(fileDescriptor1[0]);
        printf("La somma è: %d\n", result);
    }
    return 0;
}
