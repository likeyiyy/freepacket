/*************************************************************************
	> File Name: includes.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:48 PM CST
 ************************************************************************/
#ifndef INCLUDES_H
#define INCLUDES_H
#include <stdio.h>       
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <linux/if.h> 
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/if_ether.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#define bool _Bool
#define true 1
#define false 0
//#define LIKEYI_DEBUG
#ifdef LIKEYI_DEBUG
#define DEBUG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"\n", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(format,...)  
#endif
#define SESSION_BUFFER_SIZE 65536 
#define MANAGER_QUEUE_LENGTH 50000
#define SESSION_POOL_LENGTH  10000
#define MAX_FACTOR           0.75
#define DESTORY_TIME         100000000
#include "pool_manager.h"
/*
* 一个数据包却要有数据部分和长度部分，真的必要吗？
* 是的，对于generator来说确实不必要，但是对于parser却是需要的。
* 因为parser可以从网卡和generator取得数据。
* */
typedef struct packet
{
    unsigned char * data;   /* 一个包的数据部分*/
    unsigned int    length; /* 一个包的长度*/
    pool_t * pool;          /* 这个包来自哪个池子*/
}packet_t;
typedef struct flow_item
{
    packet_t * packet;
    uint32_t upper_ip;
    uint32_t lower_ip;
    uint32_t upper_port;
    uint32_t lower_port;
    uint8_t  protocol;
    unsigned char * payload;
    unsigned int    payload_len;
    pool_t * pool;          /* 这个包来自哪个池子*/
}flow_item_t;
typedef struct _session_item
{
    uint8_t buffer[SESSION_BUFFER_SIZE];
    uint64_t last_time;
    uint32_t length;
    uint32_t cur_len;
    uint32_t upper_ip;
    uint32_t lower_ip;
    uint32_t upper_port;
    uint32_t lower_port;
    uint8_t  protocol;
    pool_t * pool;
}session_item_t;

#define INTEL YES
#ifdef INTEL
static inline uint64_t get_cycle_count_intel()
{
    unsigned int lo,hi; \
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
    return ((uint64_t)hi << 32) | lo; \
}
#define GET_CYCLE_COUNT()  get_cycle_count_intel()
#elif defined TILERA
#include <arch/cycle.h>
#define GET_CYCLE_COUNT() get_cycle_count()
#else
#error "get_cycle_count not define"
#endif

#include "config.h"
#include "parser_queue.h"
#include "checksum.h"
#include "manager_queue.h"
#include "list.h"
#include "hash.h"
#include "packet_manager.h"
#include "packet_parser.h"
#include "parse.h"
#include "packet_generator.h"
#include "display.h"
#endif
