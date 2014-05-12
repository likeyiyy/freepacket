/*************************************************************************
	> File Name: packet_generator.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:21 PM CST
 ************************************************************************/
#ifndef PACKET_GENERATOR_H
#define PACKET_GENERATOR_H
typedef struct generator
{
    pthread_t   id;
    int         index;
    int         next_thread_id;
    uint64_t    total_send_byte;
    config_t  * config;
    pool_t    * pool;
    parser_set_t * parser_set;
    struct timeval old,now;
}generator_t;
typedef struct generator_info
{
    generator_t * generator;
    uint32_t numbers;
    config_t  * config;
    pool_t    * pool;
}generator_info_t;
extern generator_info_t * generator_info;
extern config_t * config;
void   init_generator(int numbers);
/*
 * 销毁线程，和销毁线程的数据结构要分两步走。
 * 尤其是当这个线程和其他线程有交互时。
 * */
void   destroy_generator(generator_info_t * generator_info);
void   finish_generator(generator_info_t * generator_info);
void * packet_generator_loop(void * arg);
#endif
