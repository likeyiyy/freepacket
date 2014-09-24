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
#include <getopt.h>
#include <pthread.h>
#include <malloc.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
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


#ifdef TILERA_PLATFORM
#include <sys/mman.h>
#include <sys/dataplane.h>
#include <tmc/alloc.h>
#include <arch/atomic.h>
#include <arch/sim.h>

#include <gxio/mpipe.h>
#include <tmc/cpus.h>
#include <tmc/mem.h>
#include <tmc/spin.h>
#include <tmc/sync.h>
#include <tmc/task.h>
#include <tmc/perf.h>
#endif


#define PROCBLOCK_SIZE 32
#define PAGE_SHIFT     12

#define bool _Bool
#define true 1
#define false 0
//#define LIKEYI_DEBUG
#ifdef LIKEYI_DEBUG
#define DEBUG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"\n", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(format,...)  
#endif
#define COLLECTOR_MODE 0
#define GENERATOR_MODE 1

#ifdef TILERA_PLATFORM
/* Align "p" mod "align", assuming "p" is a "void*". */
#define ALIGN(p, align) do { (p) += -(long)(p) & ((align) - 1);  } while(0)
/* Help check for errors. */
#define VERIFY(VAL, WHAT)                                    \
        do {                                                 \
        long long __val = (VAL);                             \
            if (__val < 0)                                   \
            tmc_task_die("Failure in '%s': %lld: %s.",       \
                (WHAT), __val, gxio_strerror(__val));        \
        } while (0)
#endif

#define WAIT_MODE    0
#define NO_WAIT_MODE 1


#define LG2_CAPACITY                (10)
#define MANAGER_POOL                (16)
static inline void exit_if_ptr_is_null(void * ptr,const char * message)   
{                                                           
    if(ptr == NULL)                                         
    {                                                       
        printf("%s\n",message);                             
        exit(-1);                                           
    }                                                       
}

#define unlikely(cond)        __builtin_expect((cond), 0)
#define likely(cond)          __builtin_expect((cond), 1)

#ifdef TILERA_PLATFORM
#include "queue_tilegx.h"
TMC_QUEUE(swsr_queue, void *, LG2_CAPACITY, (TMC_QUEUE_SINGLE_SENDER | TMC_QUEUE_SINGLE_RECEIVER));
TMC_QUEUE(mwsr_queue, void *, LG2_CAPACITY, (TMC_QUEUE_SINGLE_RECEIVER));
TMC_QUEUE(swsr_pool, void *, MANAGER_POOL, (TMC_QUEUE_SINGLE_SENDER | TMC_QUEUE_SINGLE_RECEIVER));
TMC_QUEUE(mwsr_pool, void *, LG2_CAPACITY, (TMC_QUEUE_SINGLE_RECEIVER));
#else
#include "queue_intel.h"
INTEL_QUEUE(swsr_queue, void *, LG2_CAPACITY, (TMC_QUEUE_SINGLE_SENDER | TMC_QUEUE_SINGLE_RECEIVER));
INTEL_QUEUE(mwsr_queue, void *, LG2_CAPACITY, (TMC_QUEUE_SINGLE_RECEIVER));
INTEL_QUEUE(swsr_pool, void *, MANAGER_POOL, (TMC_QUEUE_SINGLE_SENDER | TMC_QUEUE_SINGLE_RECEIVER));
INTEL_QUEUE(mwsr_pool, void *, LG2_CAPACITY, (TMC_QUEUE_SINGLE_RECEIVER));
#endif
/*
* 一个数据包却要有数据部分和长度部分，真的必要吗？
* 是的，对于generator来说确实不必要，但是对于parser却是需要的。
* 因为parser可以从网卡和generator取得数据。
* */
typedef struct packet
{
    unsigned char * data;   /* 一个包的数据部分*/
    unsigned int    length; /* 一个包的长度*/
    mwsr_pool_t * pool;          /* 这个包来自哪个池子*/
}packet_t;
typedef struct flow_item
{
    uint32_t upper_ip;
    uint32_t lower_ip;
    uint32_t upper_port;
    uint32_t lower_port;
    uint8_t  protocol;
    packet_t * packet;
    unsigned char * payload;
    unsigned int    payload_len;
    mwsr_pool_t * pool;          /* 这个包来自哪个池子*/
}flow_item_t;
typedef struct _session_item
{
    uint32_t upper_ip;
    uint32_t lower_ip;
    uint32_t upper_port;
    uint32_t lower_port;
    uint8_t  protocol;
    uint8_t  * buffer;
    uint64_t last_time;
    uint32_t length;
    uint32_t cur_len;
    swsr_pool_t * pool;
}session_item_t;

#ifdef INTEL_PLATFORM
static inline uint64_t get_cycle_count_intel()
{
    unsigned int lo,hi; \
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
    return ((uint64_t)hi << 32) | lo; \
}
#define GET_CYCLE_COUNT()  get_cycle_count_intel()


#elif defined TILERA_PLATFORM
#include <arch/cycle.h>
#define GET_CYCLE_COUNT() get_cycle_count()

#else
#error "get_cycle_count not define"
#endif


#include "config.h"
#include "mpipe.h"
#include "checksum.h"
#include "list.h"
#include "hash.h"

#include "lossratio.h"
#include "packet_manager.h"
#include "packet_parser.h"
#include "parse.h"
#include "packet_generator.h"
#include "display.h"

#ifdef TILERA_PLATFORM
extern cpu_set_t global_cpus;
extern tmc_sync_barrier_t gbarrier; 
#endif
extern int ghash_view[36];
#define FULL_SPEED 	0
#define USER_SPEED 	1
#define IP_IDX    	16
#define TCP_IDX          36
#define UDP_IDX          36
#define PAY_LEN 	2

#endif
