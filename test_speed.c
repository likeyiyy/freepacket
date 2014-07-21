/*************************************************************************
	> File Name: test_speed.c
	> Author: likeyi
	> Mail: likeyi@sina.com 
	> Created Time: Mon 21 Jul 2014 03:19:14 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static int get_update_speed(char * buf)
{
    int i = 0;
    while(!isdigit(*(buf+i)))
    {
        i++;
    }
    printf("%s,%s\n",buf,buf+i);
    int speed = atoi(buf+i);
    return speed;
}

int main()
{
    int k = get_update_speed("SYN_OK:SET_SPEED:20");
    printf("k :[%d]\n",k);
}
