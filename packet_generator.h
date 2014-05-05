/*************************************************************************
	> File Name: packet_generator.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:21 PM CST
 ************************************************************************/

#ifndef PACKET_GENERATOR_H
#define PACKET_GENERATOR_H
typedef struct config
{
    int TCP;
    int UDP;
    uint32_t saddr;
    uint32_t daddr;

    uint32_t saddr_counter;
    uint32_t daddr_counter;

    uint16_t sport;
    uint16_t dport;

    uint16_t sport_counter;
    uint16_t dport_counter;

    uint32_t speed;

    int numbers;   /* PACKET pool 的大小*/
    int packet_size;
}config_t;
typedef struct generator
{
    config_t  * config;
    pool_t    * pool;
    manager_t * manager;
}generator_t;
typedef struct generator_info
{
    generator_t * generator;
    uint32_t numbers;
}generator_info_t;

void   init_generator(int numbers);
void * packet_generator_loop(void * arg);
#endif
