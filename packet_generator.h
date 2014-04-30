/*************************************************************************
	> File Name: packet_generator.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:21 PM CST
 ************************************************************************/

#ifndef PACKET_GENERATOR_H
#define PACKET_GENERATOR_H
typedef struct _packet_pool
{

}packet_pool_t;

packet_pool_t * init_packet_pool(unsigned int size);
packet_pool_t * get_packet_pool();
#endif
