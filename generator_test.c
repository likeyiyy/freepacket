/*************************************************************************
	> File Name: generator_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:40 PM CST
 ************************************************************************/

#include "includes.h"

int main()
{
    /*
    * 初始化几个线程，packet_parser。
    * */
    init_packet_parse(PACKET_PARSER_NUMS);

    /*
    * 初始化产生数据包的线程。
    * */
    init_generator(PACKET_GENERATOR_NUMS);

    while(1)
    {
        sleep(10);
    }
    pthread_exit(NULL);
}

