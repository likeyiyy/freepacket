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
typedef struct top_config
{
    int pid;
    char * pname;
    int time;
    int time_flag;
    int pid_flag;
    int pname_flag;
    int conflict_flag;
}config_s;

int top_read_config_file(const char * file_name,config_s * config);

#endif
