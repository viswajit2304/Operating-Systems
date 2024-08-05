#include<stdlib.h>
#include<stdio.h>

int main()
{
    int *data = (int *)malloc(100*sizeof(int));
    data[100] = 0;
}