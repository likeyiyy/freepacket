/*************************************************************************
	> File Name: session.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 12 May 2014 10:41:44 AM CST
 ************************************************************************/
#ifndef H_SESSION_H
#define H_SESSION_H
#define SESSION_BUFFER_SIZE 65536 
#define SESSION_QUEUE_LENGTH 50000
#define SESSION_POOL_LENGTH  10000
#define MAX_FACTOR           0.75
#define DESTORY_TIME         1
typedef struct _session_item
{
    uint8_t buffer[SESSION_BUFFER_SIZE];
    struct timeval last_time;
    uint32_t length;
    uint32_t cur_len;
    uint32_t upper_ip;
    uint32_t lower_ip;
    uint32_t upper_port;
    uint32_t lower_port;
    uint8_t  protocol;
}session_item_t;
typedef struct _session
{
    struct _session * next;
    struct _session * prev;
    session_item_t  item;
}session_t;
typedef struct _session_bucket
{
    pthread_t         id;
    int               index;
    session_t       * list;
    pool_t          * session_pool;
    session_queue_t * queue;
}bucket_t;
typedef struct _set
{
    bucket_t * bucket;
    uint32_t    length;
}session_set_t;
/*
* 初始化一个session工作组。
* */
session_set_t * init_session_set(uint32_t length);
/*
* 真正的工作者。
* */
void * session_worker(void * arg);
#endif
