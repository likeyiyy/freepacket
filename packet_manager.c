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
        set->manager[i].queue = init_session_queue(SESSION_QUEUE_LENGTH,sizeof(flow_item_t));
        set->manager[i].list  = malloc(sizeof(struct list_head));
        exit_if_ptr_is_null(set->manager[i].list,"alloc list head error\n");
        INIT_LIST_HEAD(set->manager[i].list);
        set->manager[i].session_pool = init_pool(SESSION_POOL,
                                                SESSION_POOL_LENGTH,
                                                sizeof(struct blist));
        set->manager[i].session_pool->pool_type = 2;
        set->manager[i].index = i;
        set->manager[i].list_length = 0;
        pthread_mutex_init(&set->manager[i].list_lock,NULL);
    }
    for(i = 0; i < length; i++)
    {
        pthread_create(&set->manager[i].id,
                       NULL,
                     session_worker,
                      &set->manager[i]);
    }
    return set;
}
static inline int compare_flow(session_item_t * item , flow_item_t * flow)
{
    session_item_t * session = item;
    if( session->protocol == flow->protocol && 
        session->upper_ip == flow->upper_ip &&
        session->lower_ip == flow->lower_ip &&
        session->upper_port == flow->upper_port &&
        session->lower_port   == flow->lower_port) 
    {
        return 0;
    }
    return 1;
}
static struct blist * find_list(struct list_head * head, flow_item_t * flow)
{
    struct list_head * p;
    struct blist * node;
    list_for_each(p,head)
    {
        node = list_entry(p,struct blist,listhead);
        if(compare_flow(&node->item,flow) == 0)
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
static inline void make_new_session(struct blist * blist,flow_item_t * flow)
{
    session_item_t * item = &blist->item;
    item->length     = SESSION_BUFFER_SIZE;
    item->cur_len    = 0;
    item->upper_ip   = flow->upper_ip;
    item->lower_ip   = flow->lower_ip;
    item->upper_port = flow->upper_port;
    item->lower_port = flow->lower_port;
    item->protocol   = flow->protocol;
    gettimeofday(&item->last_time,NULL);
    memcpy(item->buffer,flow->payload,flow->payload_len);
    item->cur_len  += flow->payload_len;
}
void delete_session(manager_t * manager)
{
    struct list_head * p;
    struct blist * node;
    struct list_head * next;
    struct timeval current_time;
    gettimeofday(&current_time,NULL);
    list_for_each_safe(p,next,manager->list)
    {
        node = list_entry(p,struct blist,listhead); 
        if((1.0 * node->item.cur_len > MAX_FACTOR * node->item.length) ||
        (current_time.tv_sec - node->item.last_time.tv_sec > DESTORY_TIME))
        {
            list_del(&node->listhead);
            --manager->list_length;
            free_buf(manager->session_pool,(void *)node); 
        }
    }
}
void * process_session(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    while(1)
    { 
        pthread_mutex_lock(&manager->list_lock);
        delete_session(manager);
        pthread_mutex_unlock(&manager->list_lock);
        usleep(50*1000);
    }
}
/*
* 真正的工作者。
* */
void * session_worker(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    flow_item_t * flow;
    pthread_t clean_id;
    pthread_create(&clean_id,
                  NULL,
                 process_session,
                  arg);
    struct blist * blist;
    struct blist * new_blist;
    while(1)
    {
        /*
         * 只有确实有数据时才返回。
         * */
        pop_session_buf(manager->queue,(void **)&flow);
        pthread_mutex_lock(&manager->list_lock);
        blist = find_list(manager->list,flow);
        /*
        * Not find the entry,Create it and add to tail.
        * */
        if(!blist)
        {
            get_buf(manager->session_pool,(void **)&new_blist);
            make_new_session(new_blist,flow);
            free_flow(flow);
            INIT_LIST_HEAD(&new_blist->listhead);
            list_add_tail(&new_blist->listhead,manager->list);
            ++manager->list_length;
        }
        /* 
        * Found it, and memcpy it.
        * */
        else
        {
            session_item_t * session = &blist->item;
            if(session->cur_len+flow->payload_len < session->length)
            {
                memcpy(session->buffer+session->cur_len,flow->payload,flow->payload_len);
                session->cur_len += flow->payload_len;
                gettimeofday(&session->last_time, NULL);
            }
            free_flow(flow);
        }
        pthread_mutex_unlock(&manager->list_lock);
    }
}
