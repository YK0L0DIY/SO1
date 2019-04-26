#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
//    pid_t pid, pid2;

    fork(); //cria um prosses pai e um filho =2
    fork(); //cria um prosses pai e um filho =4
    fork(); //cria um prosses pai e um filho =8

    puts("Hello");

    return 0;
}