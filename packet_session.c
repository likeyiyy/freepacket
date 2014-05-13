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
session_set_t * session_set;
session_set_t * init_session_set(uint32_t length)
{
    session_set_t * set = malloc(sizeof(session_set_t));
    session_set = set;
    exit_if_ptr_is_null(set,"初始化分配流哈希错误");
    set->bucket = malloc(sizeof(bucket_t) * length);
    exit_if_ptr_is_null(set->bucket,"初始化分配bucket错误");
    set->length = length;
    int i = 0;
    for(i = 0; i < length; i++)
    {
        set->bucket[i].queue = init_session_queue(SESSION_QUEUE_LENGTH,sizeof(flow_item_t));
        set->bucket[i].list  = NULL;
        set->bucket[i].session_pool = init_pool(SESSION_POOL,
                                                SESSION_POOL_LENGTH,
                                                sizeof(session_t));
        set->bucket[i].session_pool->pool_type = 2;
        set->bucket[i].index = i;
    }
    for(i = 0; i < length; i++)
    {
        pthread_create(&set->bucket[i].id,
                       NULL,
                     session_worker,
                      &set->bucket[i]);
    }
    return set;
}
static inline int compare_flow(session_t * list , flow_item_t * flow)
{
    session_item_t * session = &list->item;
    if(session->protocol == flow->protocol && (
            (session->saddr == flow->saddr &&
            session->daddr == flow->daddr &&
            session->source == flow->source &&
            session->dest   == flow->dest)||
            (session->saddr == flow->daddr &&
             session->source == flow->dest &&
             session->daddr == flow->saddr &&
             session->dest  == flow->source)
            )
      )
    {
        return 0;
    }
    return 1;
}
static inline void free_flow(flow_item_t * flow)
{
                /* 
                 * 注意这两个free
                 * */
    free_buf(flow->packet->pool,flow->packet);
    free_buf(flow->pool,flow);
}
static inline void make_new_session(session_t * new_session,flow_item_t * flow)
{
    session_item_t * item = &new_session->item;
    item->length    = SESSION_BUFFER_SIZE;
    item->cur_len   = 0;
    item->saddr     = flow->saddr;
    item->daddr     = flow->daddr;
    item->source    = flow->source;
    item->dest      = flow->dest;
    item->protocol  = flow->protocol;
    gettimeofday(&item->last_time,NULL);
    memcpy(item->buffer,flow->payload,flow->payload_len);
    free_flow(flow);
    item->cur_len  += flow->payload_len;
}
/*
* 真正的工作者。
* */
void * session_worker(void * arg)
{
    bucket_t * bucket = (bucket_t *)arg;
    flow_item_t * flow;
    session_t * head, * next, * prev;
    while(1)
    {
        /*
         * 只有确实有数据时才返回。
         * */
        pop_session_buf(bucket->queue,(void **)&flow);
        prev = next = bucket->list;
        while(next != NULL)
        {
            /*
             * 相等返回0，不想等返回非零。
             * */
            if(compare_flow(next,flow) == 0)
            {
                /* Do some thing. */
                session_item_t * session = &next->item;
                /*
                 * Why use payload but still have packet?
                 * because it usefull.
                 * */
                memcpy(session->buffer+session->cur_len,flow->payload,flow->payload_len);
                free_flow(flow);
                session->cur_len += flow->payload_len;
                gettimeofday(&session->last_time, NULL);
                break;
            }
            prev = next;
            next = next->next;
        }
        session_t * new_session;
        if(next == NULL)
        {
            /* Add new item */
            get_buf(bucket->session_pool,(void **)&new_session);
            new_session->next = NULL;
            new_session->prev = prev;
            make_new_session(new_session,flow);
            /*
            * 这说明是头结点。头结点，next也是NULL.
            * */
            if(prev == NULL)
            {
                bucket->list = new_session;
            }
        }
        next = bucket->list;
        struct timeval current_time;
        while(next != NULL)
        {
            head = next->next;
            gettimeofday(&current_time,NULL);
            if((1.0 * next->item.cur_len > MAX_FACTOR * next->item.length) ||
                (current_time.tv_sec - next->item.last_time.tv_sec > DESTORY_TIME))
            {
                free_buf(bucket->session_pool,(void *)next); 
            }
            /* Do check here */
            next = head; 
        }
    }
}
