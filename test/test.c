#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char *expresion = "(6+4)";
    int result;

    int r = syscall(439, expresion, strlen(expresion), &result);
    printf("Respuesta: %d\n", result);
    printf("Sys r: %d\n", r);
    return 0;
}
