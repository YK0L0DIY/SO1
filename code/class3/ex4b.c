#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    pid_t pid;

    pid = fork();
    if (pid > 0) {
        fork();
        fork();
        puts("Hello1");
    }

    puts("Hello2");
}