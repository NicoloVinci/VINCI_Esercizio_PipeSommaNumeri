#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main(void) {
    int fileDescriptor[2];
    if (pipe(fileDescriptor) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t child;
    if ((child = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child == 0) {
        ssize_t n;
        int num1, num2, num, i = 0, sum = 0;
        while ((n = read(fileDescriptor[0], &num, sizeof(int))) > 0) {
            if (n != sizeof(int)) {
                perror("read from father");
                exit(EXIT_FAILURE);
            }
            if (i == 0) {
                num1 = num;
            } else {
                num2 = num;
            }
            i++;
        }
        if (n == -1) {
            perror("read from father");
            exit(EXIT_FAILURE);
        }
        if (i != 2) {
            printf("Nessun numero ricevuto.\n");
            close(fileDescriptor[0]);
            exit(EXIT_FAILURE);
        }
        close(fileDescriptor[0]);
        sum = num1 + num2;
    } else {
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
            if (i == 0) {
                a = (int)val;
            } else {
                b = (int)val;
            }
        }
        for (int i = 0; i < 2; i++) {
            if (i == 0) {
                if (write(fileDescriptor[1], &a, sizeof(int)) != sizeof(int)) {
                    perror("write to child");
                    i--;
                    return 1;
                }
            } else {
                if (write(fileDescriptor[1], &b, sizeof(int)) != sizeof(int)) {
                    perror("write to child");
                    i--;
                    return 1;
                }
            }
        }
        close(fileDescriptor[1]);
        wait(NULL);
        int result;
        if (read(fileDescriptor[0], &result, sizeof(int)) != sizeof(int)) {
            perror("read from child");
            return 1;
        }
        close(fileDescriptor[0]);
        printf("La somma è: %d\n", result);
    }
    return 0;
}