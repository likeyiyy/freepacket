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
#define PROCBLOCK_SIZE 32
#define PAGE_SHIFT     12

#define PACKET_POOL_SIZE 10000
#define TCP 1
#define UDP 2
typedef struct config
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

    uint16_t sport_min;
    uint16_t dport_min;

    uint16_t sport_cur;
    uint16_t dport_cur;

    uint16_t sport_max;
    uint16_t dport_max;

    uint32_t speed;

    unsigned char * pkt_data;

    int numbers;   /* PACKET pool 的大小*/
    int pktlen;
}config_t;

int ether_atoe(const char *a, unsigned char *e);

char * ether_etoa(const unsigned char *e, char *a);

int read_config_file(const char * file_name,config_t * config);

void print_config_file(config_t * config);
#endif
