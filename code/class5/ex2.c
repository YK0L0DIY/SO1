#include <pthread.h>
#include <math.h>
#include <stdio.h>

double soma1=0;
double soma2=0;
double n=  1000000;
double meio=500000;

void* tread1 (void* unused)
{
    for(double i=0;i<=meio;i++) {
        soma1+=sqrt(i);
    }
    return NULL;
}

void* tread2 (void* unused)
{
    for(double i=n;i>meio;i--) {
        soma2+=sqrt(i);
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

    printf("%f\n",soma1+soma2);

    return 0;
}