#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
void main()
{
    char *m = (char *)malloc(sizeof(char));
    *m = 5;
    printf("Virtual address is %lu\n",m);
    printf("Process id is %d\n",getpid());
    sleep(30);
}