/*************************************************************************
	> File Name: malloc_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 06:01:56 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "includes.h"
unsigned long count = 0;

void sigfun(int a)
{
    printf("total alloc %d Bytes %d MByte\n",count,(count/1024)/1024);
    exit(0);
}
int main()
{
    signal(SIGALRM,sigfun);
    alarm(1);
    void * p;
    while(1)
    {
        p = malloc(1514);
        count += 1514;
        free(p);
    }


}
