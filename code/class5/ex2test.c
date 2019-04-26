#include <pthread.h>
#include <math.h>
#include <stdio.h>

double soma=0;
double n=  1000000;
double meio=500000;

void* tread1 (void* unused)
{
    int raiz;
    for(double i=0;i<=meio;i++) {
        raiz=sqrt(i);
        soma+=raiz;
    }
    return NULL;
}

void* tread2 (void* unused)
{
    int raiz;
    for(double i=n;i>meio;i--) {
        raiz=sqrt(i);
        soma+=raiz;
    }
    return NULL;
}

int main ()
{
    pthread_t thread_id,thread_id2;

    pthread_create(&thread_id, NULL, &tread1, NULL);
    pthread_create(&thread_id2, NULL, &tread2, NULL);

    pthread_join(thread_id,NULL);
    pthread_join(thread_id2,NULL);

    printf("%f\n",soma);

    return 0;
}