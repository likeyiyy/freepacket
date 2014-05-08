/*************************************************************************
	> File Name: pool_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 05:19:16 PM CST
 ************************************************************************/

#include <stdio.h>
#include "includes.h"

int main(int argc,char ** argv)
{
    pool_type_t type = PACKET_POOL;

    pool_t * pool = init_pool(type,100,sizeof(packet_t));
    pool_t * pool2 = get_pool(type);
#if 0
    int i = 0;
    for(i = 0; i <99; ++i)
    {
        pop_pool(pool2,(void **)&item_a[i]);
        item_a[i]->a = i;
        item_a[i]->b = 100 - i;
        sprintf(item_a[i]->c,"hello %d times\n",i);
        printf("pool free num %d\n",pool2->free_num);
    }

    for(i = 0; i < 100; ++i)
    {
        item_t * hello = (item_t *)(pool2->buffer + i * sizeof(item_t));        
        printf("a;%d,b:%d,c:%s\n",hello->a,hello->b,hello->c);
    }
    for(i = 0; i < 100; ++i)
    {
        item_t * hello = (item_t *)*(pool2->node + i);        
        printf("a;%d,b:%d,c:%s\n",hello->a,hello->b,hello->c);
    }
    #endif
}
