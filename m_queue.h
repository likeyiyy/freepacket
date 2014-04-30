/*************************************************************************
	> File Name: m_queue.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Fri 25 Apr 2014 09:29:03 AM CST
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
    unsigned long long  length;
    pthread_mutex_t lock;
}queue_t;
/*
* 不设长度限制.
* */
queue_t * init_queue();
int is_empty(queue_t * q);
int push_to_queue(queue_t * q, void * data);
int pop_from_queue(queue_t * q,void ** data);

#endif
