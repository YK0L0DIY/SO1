#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_COUNT 200

void ChildProcess(void); /* child process prototype */
void ParentProcess(void); /* parent process prototype */

int temp = 1;
int n=5;

int main(void) {
    pid_t pid;

    pid = fork();
    if (pid == 0){
        ChildProcess();
        printf("main\n");
    }
    printf("acabou filho\n");

    return 0;
}

void ChildProcess(void){
    if (n == 0) {
        printf("resultado: %d\n", temp);
    } else {
        pid_t pid;
        temp *= n;
        printf("%d\n",temp);
        n--;
        pid = fork();
        if (pid == 0) {
            ChildProcess();
        }
    }
    printf("fim child\n");
}

void ParentProcess(void) {
    int i;

    for (i = 1; i <= MAX_COUNT; i++)
        printf("This line is from parent, value = %d\n", i);
    printf("*** Parent is done ***\n");
}
