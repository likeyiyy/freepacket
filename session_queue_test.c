/*************************************************************************
	> File Name: session_queue_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 12 May 2014 10:18:44 AM CST
 ************************************************************************/
#include "includes.h"
typedef struct test
{
    char c [100];
    double b;
}test_t;
int main(int argc,char ** argv)
{
    session_queue_t * session = init_session_queue(100,sizeof(test_t));
    int i = 99;
    while(i--)
    {
        test_t * test = malloc(sizeof(test_t));
        test->b = i + 43.34354;
        sprintf(test->c,"I love you %d times",i);
        push_buf(session,(void *)test);
    }
    i = 120;
    while(i--)
    {
        test_t * test;
        pop_buf(session,(void **)&test);
        printf("b:%f,c:%s\n",test->b,test->c);
    }
}
