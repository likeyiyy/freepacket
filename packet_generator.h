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
    uint64_t    drop_total;
    sim_config_t  * config;
    pool_t    * pool;
    struct timeval old,now;
    int rank;
    mpipe_common_t * mpipe;
}generator_t;
typedef struct generator_set
{
    generator_t * generator;
    uint32_t numbers;
    sim_config_t  * config;
}generator_set_t;


generator_set_t * get_generator_set();
generator_set_t * init_generator_set(sim_config_t * config);
/*
 * 销毁线程，和销毁线程的数据结构要分两步走。
 * 尤其是当这个线程和其他线程有交互时。
 * */
void   destroy_generator(generator_set_t * generator_set);
void   finish_generator(generator_set_t * generator_set);
#endif
