/**
=========================================================================
 Author: findstr
 Email: findstr@sina.com
 File Name: config.h
 Description: (C)  2014-03  findstr
 Edit History: 
   2014-03-06    File created.
=========================================================================
**/
#ifndef CONFIG_H_H
#define CONFIG_H_H
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <dirent.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

#define CONFIG_FILE "./generator.conf"

#define GENERATOR_POOL_SIZE 100000

#define PARSER_QUEUE_LENGTH 50000
#define PARSER_POOL_SIZE    100000

#define MANAGER_QUEUE_LENGTH 50000
#define MANAGER_POOL_SIZE    50000
/* a primer*/
#define MANAGER_HASH_LENGTH  1173
#define MANAGER_BUFFER_SIZE  65536

#define MAX_FACTOR           0.75
/* nanosecond */
#define DESTORY_TIME         100000000

typedef struct __config
{
    int protocol;
    unsigned char srcmac[6];
    unsigned char dstmac[6];
    uint32_t saddr_min;
    uint32_t daddr_min;
    uint32_t saddr_cur;
    uint32_t daddr_cur;
    uint32_t saddr_max;
    uint32_t daddr_max;
    uint32_t sport_min;
    uint32_t dport_min;
    uint32_t sport_cur;
    uint32_t dport_cur;
    uint32_t sport_max;
    uint32_t dport_max;
    uint32_t speed;
    char * pkt_data;
    int pktlen;
    uint32_t period;
    char * link_name;
    int     packet_generator_mode;
#ifdef TILERA_PLATFORM
    int32_t  notif_ring_entries;
    uint32_t equeue_entries;
    int     per_worker_buckets;
	int     once_packet_nums;
#endif

    int     generator_nums;
    int     parser_nums;
    int     manager_nums;

    int generator_pool_size;   

    int parser_queue_length;
    int parser_pool_size;

    int manager_queue_length;
    int manager_pool_size;
    int manager_hash_length;
    int manager_buffer_size;

	int pipe_depth;
	int screen_display;
	int speed_mode;
}sim_config_t;

extern sim_config_t * global_config;

int ether_atoe(const char *a, unsigned char *e);

char * ether_etoa(const unsigned char *e, char *a);

int read_config_file(const char * file_name, sim_config_t * config);

int read_config_simple(const char * file_name,sim_config_t * config);

int init_config_s(sim_config_t * config);

void print_config_file(sim_config_t * config);

void exit_if_ptr_is_null(void * p,const char * c);

uint32_t calc_period(double length,double rate,uint32_t thread_num);
#endif
