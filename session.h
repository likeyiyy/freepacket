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
typedef struct _session
{
    uint8_t buffer[SESSION_BUFFER_SIZE];
    struct timeval last_time;
    uint32_t length;
    uint32_t cur_len;
    uint32_t saddr;
    uint32_t daddr;
    uint32_t source;
    uint32_t dest;
    uint8_t  protocol;
}session_t;

typedef struct _flow_list
{
    struct _flow_list * next;
    struct _flow_list * prev;
    session_t  session;
}flow_list_t;

typedef struct _session_bucket
{
    flow_list_t 　* list;
    session_queue_t * queue;
    pool_t        * pool;
}bucket_t;

typedef struct _hash
{
    bucket_t * bucket;
    uint32_t    length;
}session_hash_t;

/*
* 初始化一个session工作组。
* */
session_hash_t * init_sessino_hash(uint32_t length);
/*
* 真正的工作者。
* */
void * session_worker(void * arg);
#endif
