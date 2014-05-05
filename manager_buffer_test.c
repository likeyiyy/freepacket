/*************************************************************************
	> File Name: manager_buffer_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 05 May 2014 11:26:48 AM CST
 ************************************************************************/

#include <stdio.h>
#include "includes.h"
typedef struct 
{
    char c[20];
    int a;
}item_t;
int main(int argc,char ** argv)
{
    queue_t * q = init_queue(10001);
    int i = 0;
    item_t * t;
    for(i = 0; i < 10000; ++i)
    {
        t = malloc(sizeof(item_t));
        t->a = i;
        sprintf(t->c,"The %d one",i);
        printf("%d\n",i);
        push_to_queue(q,(void *)t);
    }

    for(i = 0; i < 10000; ++i)
    {
        pop_from_queue(q,(void **)&t);
        printf("t->a:%d t->c:%s\n",t->a,t->c);
    }

    return 0;
}
