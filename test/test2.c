#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s expresion_matematica\n", argv[0]);
        return 1;
    }

    char *expresion = argv[1];
    int result;

    int r = syscall(439, expresion, strlen(expresion), &result);
    printf("Respuesta: %d\n", result);
    printf("Sys r: %d\n", r);

    return 0;
}
