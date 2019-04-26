#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void forkTest() {
    puts("TESTE FORK:\n");

    pid_t pid;
    for (int i = 1; i <= 2; i++) {
        pid = fork(); //cria processo

        if (pid == 0) {
            printf("B");
        } else {
            printf("A");
        }
    }
}

int main() {
    forkTest();
    return 0;
}
