/*************************************************************************
	> File Name: packet_generator.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:21 PM CST
 ************************************************************************/

#ifndef PACKET_GENERATOR_H
#define PACKET_GENERATOR_H
#include "includes.h"
typedef struct generator
{
    config_t  * config;
    pool_t    * pool;
}generator_t;
typedef struct generator_info
{
    generator_t * generator;
    uint32_t numbers;
}generator_info_t;

void   init_generator(int numbers);
void * packet_generator_loop(void * arg);
#endif
