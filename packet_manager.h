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
    swsr_pool_t       * session_pool;
    mwsr_queue_t      * queue;
	int alive;
    uint64_t drop_cause_pool_empty;
}manager_t;
typedef struct _group
{
    manager_t * manager;
    uint32_t    numbers;
}manager_group_t;


manager_group_t * get_manager_group();
/*
* 初始化一个session工作组。
* */
manager_group_t * init_manager_group(sim_config_t * config);

struct blist * find_list(struct list_head * head, flow_item_t * flow);
void delete_session(hash_table * ht,bucket_t * bucket);
#endif
