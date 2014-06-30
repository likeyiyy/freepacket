/*************************************************************************
	> File Name: session.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 12 May 2014 10:41:44 AM CST
 ************************************************************************/
#ifndef H_SESSION_H
#define H_SESSION_H
/*
typedef struct _session
{
    struct _session * next;
    struct _session * prev;
    session_item_t  item;
}session_t;
*/
typedef struct _session_manager
{
    pthread_t          id;
    int                index;
    struct hash_table * ht;
    pool_t            * session_pool;
    manager_queue_t   * queue;
    uint64_t drop_cause_pool_empty;
}manager_t;
typedef struct _set
{
    manager_t * manager;
    uint32_t    length;
}manager_set_t;
/*
* 初始化一个session工作组。
* */
manager_set_t * init_manager_set(uint32_t length);
/*
* 真正的工作者。
* */
void * packet_manager_loop(void * arg);

struct blist * find_list(struct list_head * head, flow_item_t * flow);
void delete_session(hash_table * ht,bucket_t * bucket);
#endif
