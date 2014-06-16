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
#define SESSION_QUEUE_LENGTH 50000
#define SESSION_POOL_LENGTH  10000
#define MAX_FACTOR           0.75
#define DESTORY_TIME         1
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
    struct timeval last_time;
    uint32_t length;
    uint32_t cur_len;
    uint32_t upper_ip;
    uint32_t lower_ip;
    uint32_t upper_port;
    uint32_t lower_port;
    uint8_t  protocol;
}session_item_t;

#include "config.h"
#include "node_queue.h"
#include "checksum.h"
#include "session_queue.h"
#include "list.h"
#include "packet_manager.h"
#include "packet_parser.h"
#include "parse.h"
#include "packet_generator.h"
#include "display.h"
#include "top_config.h"
#include "cpuinfo.h"
#include "meminfo.h"
#include "hash.h"
#include "top_list.h"
#include "use_file.h"
#include "taskinfo.h"
#include "ttop.h"
#endif
