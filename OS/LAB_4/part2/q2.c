#include<stdlib.h>
#include<stdio.h>

int main()
{
    int *data = (int *)malloc(100*sizeof(int));
    free(data);
    printf("data[] = %d\n",data[10]);
    return 0;
}