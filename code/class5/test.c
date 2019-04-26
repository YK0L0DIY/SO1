#include <pthread.h>
#include <math.h>
#include <stdio.h>

double soma=0;
double n=  10;
int treads=10;
int ntreadsEfectuadas=0;

void* tread (void* unused)
{
    int inicio=n/treads*ntreadsEfectuadas+1;

    int fim=inicio+n/treads;
    double somaL=0;
    for(double i=inicio;i<fim;i++) {
        somaL+=sqrt(i);
    }
    soma+=somaL;
    return NULL;
}


int main ()
{
    pthread_t thread_id;
    for (int i=0;i<treads;i++){
        pthread_create(&thread_id, NULL, &tread, NULL);
        pthread_join(thread_id,NULL);
        ntreadsEfectuadas++;
    }

//    pthread_join(thread_id,NULL);

    printf("%f\n",soma);

    return 0;
}