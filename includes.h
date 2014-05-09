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
#define LIKEYI_DEBUG
#ifdef LIKEYI_DEBUG
#define DEBUG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"\n", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(format,...)  
#endif

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

#include "config.h"
#include "node_queue.h"
#include "checksum.h"
#include "packet_parser.h"
#include "parse.h"
#include "packet_generator.h"
#endif
