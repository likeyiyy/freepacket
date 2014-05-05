/*************************************************************************
	> File Name: manager_buffer.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
    > Created Time: 2014年5月5日10:58:01
 ************************************************************************/

#ifndef MTHREAD_QUEUE_H
#define MTHREAD_QUEUE_H
#include <pthread.h>
#define LIKEYI_DEBUG
#ifdef LIKEYI_DEBUG

#define DEBUG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"\n", __LINE__, ##__VA_ARGS__)

#else
#define DEBUG(format,...)  
#endif
typedef struct _node
{
    struct _node * prev;
    struct _node * next;
    void * data;
}node_t;
typedef struct _queue 
{
    node_t * head;
    node_t * tail;
    unsigned long  length;
    pool_t * node_pool;
    pthread_mutex_t lock;
}queue_t;
#define NODE_POOL_SIZE 10000
/*
* 不设长度限制.
* */
queue_t * init_queue(unsigned int node_pool_size);
int is_empty(queue_t * q);
int push_to_queue(queue_t * q, void * data);
int pop_from_queue(queue_t * q,void ** data);

#endif
