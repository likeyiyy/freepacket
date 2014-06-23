/*************************************************************************
	> File Name: manager_buffer.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
    > Created Time: 2014年5月5日10:58:01
 ************************************************************************/

#ifndef MTHREAD_QUEUE_H
#define MTHREAD_QUEUE_H
#include <pthread.h>
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
    pthread_cond_t empty;
}queue_t;
/*
* 不设长度限制.
* */
extern struct timeval G_old;
extern struct timeval G_new;
queue_t * init_queue(unsigned int node_queue_size);
void destroy_queue(queue_t * queue);
int is_queue_empty(queue_t * q);
int push_to_queue(queue_t * q, void * data);
int pop_from_queue(queue_t * q,void ** data);

#endif
