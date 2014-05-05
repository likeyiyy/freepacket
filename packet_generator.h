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
    pool_t    * pool;
    manager_t * manager;
}generator_t;

void   init_generator(int numbers);
void * packet_generator(void * arg);
#endif
