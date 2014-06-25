/*************************************************************************
	> File Name: session.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 12 May 2014 10:41:38 AM CST
 ************************************************************************/
#include "includes.h"
/*
* 初始化一个session工作组。
* */
//#define memcpy(a,b,c) do { memcpy(a,b,c);printf("session memcpy here\n"); } while(0)
#define MAKE_HASH(v1,v2,h1,f1,f2,f3,f4,SIZE) \
(\
            v1 = f1 ^ f2,\
            v2 = f3 ^ f4,\
            h1 = v1 << 8,\
            h1 ^= v1 >> 4,\
            h1 ^= v1 >> 12,\
            h1 ^= v1 >> 16,\
            h1 ^= v2 << 6, \
            h1 ^= v2 << 10, \
            h1 ^= v2 << 14, \
            h1 ^= v2 >> 7,\
            h1%SIZE\
)
manager_set_t * manager_set;
manager_set_t * init_manager_set(uint32_t length)
{
    manager_set_t * set = malloc(sizeof(manager_set_t));
    manager_set = set;
    exit_if_ptr_is_null(set,"初始化分配流哈希错误");
    set->manager = malloc(sizeof(manager_t) * length);
    exit_if_ptr_is_null(set->manager,"初始化分配manager错误");
    set->length = length;
    int i = 0;
    for(i = 0; i < length; i++)
    {
        set->manager[i].queue = init_manager_queue(MANAGER_QUEUE_LENGTH,sizeof(flow_item_t));
        set->manager[i].ht = hash_create(1100);
        set->manager[i].session_pool = init_pool(SESSION_POOL,
                                                SESSION_POOL_LENGTH,
                                                sizeof(struct blist));
        set->manager[i].session_pool->pool_type = 2;
        set->manager[i].index = i;
    }
    for(i = 0; i < length; i++)
    {
        pthread_create(&set->manager[i].id,
                       NULL,
                     packet_manager_loop,
                      &set->manager[i]);
    }
    return set;
}
static inline int compare_session(session_item_t * item , session_item_t * session1)
{
    session_item_t * session = item;
    if( session->protocol == session1->protocol &&
        session->upper_ip == session1->upper_ip &&
        session->lower_ip == session1->lower_ip &&
        session->upper_port == session1->upper_port &&
        session->lower_port   == session1->lower_port) 
    {
        return 0;
    }
    return 1;
}
struct blist * find_list(struct list_head * head, session_item_t * session)
{
    struct list_head * p;
    struct blist * node;
    list_for_each(p,head)
    {
        node = list_entry(p,struct blist,listhead);
        if(compare_session(&node->item,session) == 0)
        {
            return node;
        }
    }
    return NULL;
}
static inline void free_flow(flow_item_t * flow)
{
    /* 
    * 注意这两个free
    * */
    free_buf(flow->packet->pool,flow->packet);
    free_buf(flow->pool,flow);
}
static inline void make_new_session(struct blist * blist,flow_item_t * flow,manager_t * manager)
{
    session_item_t * item = &blist->item;
    item->pool       = manager->session_pool;
    item->length     = SESSION_BUFFER_SIZE;
    item->cur_len    = 0;
    item->upper_ip   = flow->upper_ip;
    item->lower_ip   = flow->lower_ip;
    item->upper_port = flow->upper_port;
    item->lower_port = flow->lower_port;
    item->protocol   = flow->protocol;
    item->last_time  = GET_CYCLE_COUNT();
    memcpy(item->buffer,flow->payload,flow->payload_len);
    item->cur_len  += flow->payload_len;
}
void delete_session(hash_table * ht,bucket_t * bucket)
{
    struct list_head * p, * list;
    struct blist * node;
    struct list_head * next;
    uint64_t current_time;
    list = &bucket->list;
    current_time = GET_CYCLE_COUNT();
    list_for_each_safe(p,next,list)
    {
        node = list_entry(p,struct blist,listhead); 
        if((1.0 * node->item.cur_len > MAX_FACTOR * node->item.length) ||
        (current_time - node->item.last_time > DESTORY_TIME))
        {
            list_del(&node->listhead);
            free_buf(node->item.pool,(void *)node); 
            --bucket->count;
        }
    }
}
void * process_session(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    while(1)
    { 
        hash_travel_delete(manager -> ht);
        usleep(1000*1000);
    }
}
/*
* 真正的工作者。
* */
void * packet_manager_loop(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    flow_item_t * flow;
    session_item_t * session;
    pthread_t clean_id;
    pthread_create(&clean_id,NULL,process_session,arg);

    struct blist * new_blist;

    uint32_t v1,v2,h1,index;
    while(1)
    {
        /*
         * 只有确实有数据时才返回。
         * */
        pop_session_buf(manager->queue,(void **)&flow);
        /* 首先 copy 释放*/

        get_buf(manager->session_pool,(void **)&new_blist);

        make_new_session(new_blist, flow, manager);

        free_flow(flow);
        session = &new_blist->item;
        index = MAKE_HASH(v1,v2,h1,session->lower_port,
                session->upper_ip,
                session->upper_port,
                session->lower_ip,
                manager->ht->num_buckets);
        hash_add_item(manager->ht, index, new_blist); 

    }
}
