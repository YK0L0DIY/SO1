#include <math.h>
#include <stdio.h>

double soma=0;
double n=10;

int main (void)
{
    for(double i=0;i<=n;i++) {
        soma+=sqrt(i);
    }
    printf("%f\n",soma);
    return 0;
}