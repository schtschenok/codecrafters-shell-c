#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("$ ");

    char input[100];
    fgets(input, 100, stdin);
    return 0;
}
