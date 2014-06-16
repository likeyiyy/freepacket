/*************************************************************************
	> File Name: write_to_file.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 13 Mar 2014 11:16:34 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "use_file.h"
#include "taskinfo.h"
#include "top_list.h"
#include "top_config.h"
extern list_s * LIST;
const char * FILENAME = "./output";
static int write_task_to_file(int fd,taskinfo * task_ptr)
{
    if(write(fd,task_ptr,sizeof(*task_ptr)) == -1)
    {
        printf("failed write_to_file\n");
        return -1;
    }
    return 0;
}
void * write_to_file(void * arg)
{
    pthread_detach(pthread_self()); 
    taskinfo * task_ptr;
    int fd;
    if(access(FILENAME,F_OK) == 0)
        fd = open(FILENAME,O_WRONLY|O_APPEND);
    else
        fd = open(FILENAME,O_WRONLY|O_APPEND|O_CREAT);
    while(1)
    {
        if(!is_empty(LIST))
        {
            del_node(LIST,(void **)&task_ptr);
            //write to file
            write_task_to_file(fd,task_ptr);
            free_task(task_ptr); 
        }
        else
        {
            sleep(10);
        }
    }
    close(fd);
}
int read_from_file(void * buffer,size_t size)
{
    int fd;
    int len;
    fd = open(FILENAME,O_RDONLY);
    if(fd == -1)
    {
        printf("open %s error\n",FILENAME);
        return -1;

    }
    if((len =read(fd,buffer,size)) == -1)
    {
        printf("read %s error\n",FILENAME);
        return -1;
    }
    close(fd);
    return len; 
}
