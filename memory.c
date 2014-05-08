/*************************************************************************
	> File Name: memory.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 08 May 2014 11:05:21 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct packet
{
    unsigned int    length;
    unsigned char * data;
}packet_t;

int main()
{
    packet_t * p = malloc(1000 + sizeof(packet_t));
    if(p == NULL)
    {
        printf("malloc error\n");
        exit(0);
    }

    p->length = 1000;
    p->data   = (unsigned char *)p + sizeof(*p);
    unsigned char * c = "hello world";
    strcpy(p->data,c);
    
    printf("length:%u data:%s\n",p->length,p->data);
}
