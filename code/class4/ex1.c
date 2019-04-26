#include <pthread.h>
#include <stdio.h>

/* Prints x’s to stderr. The parameter is unused. Does not return. */
void* print_xs (void* unused)
{
    for(int i=0;i<10;i++) {
        fputc('x', stderr);
    }
    return NULL;
}

/* The main program. */
int main ()
{
    pthread_t thread_id,thread_id2;
    /* Create a new thread. The new thread will run the print_xs() function. */
    pthread_create(&thread_id, NULL, &print_xs, NULL);
    for(int i=0;i<50;i++) {
        fputc('-', stderr);
    }
    //pthread_create(&thread_id2, NULL, &print_xs, NULL);
    pthread_join(thread_id,NULL);
//    pthread_join(thread_id2,NULL);
    /* Print o’s continuously to stderr. */
    for(int i=0;i<5;i++) {
        fputc('o', stderr);
    }

    return 0;
}