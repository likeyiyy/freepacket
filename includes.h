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
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#define bool _Bool
#define true 1
#define false 0

#include "pool_manager.h"
#include "manager_buffer.h"
#endif
