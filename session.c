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
session_hash_t * init_sessino_hash(uint32_t length)
{
    session_hash_t * hash = malloc(sizeof(session_hash_t));
    exit_if_ptr_is_null(hash,"初始化分配流哈希错误");
    hash->bucket = malloc(sizeof(bucket_t) * length);
    exit_if_ptr_is_null(hash->bucket,"初始化分配bucket错误");
    hash->length = length;
    int i = 0;
    for(i = 0; i < length; i++)
    {
        hash->bucket[i].queue = init_session_queue(SESSION_QUEUE_LENGTH,sizeof(flow_item_t));
        hash->bucket[i].list  = NULL;
    }
}
static inline int compare_flow(flow_list_t * list , flow_item_t * flow)
{
    session_t * session = &list->session;
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
/*
* 真正的工作者。
* */
void * session_worker(void * arg)
{
    bucket_t * bucket = (bucket_t *)arg;
    flow_item_t * flow;
    flow_list_t * head, * next, * prev;
    while(1)
    {
        /*
         * 只有确实有数据时才返回。
         * */
        pop_buf(bucket->queue,(void **)&flow);
        next = bucket->list;
        while(next != NULL)
        {
            /*
             * 相等返回0，不想等返回非零。
             * */
            if(compare_flow(next,flow) == 0)
            {
                /* Do some thing. */
                session_t * session = &next->session;
                /*
                 * Why use payload but still have packet?
                 * because it usefull.
                 * */
                memcpy(session->buffer+session->cur_len,flow->payload,flow->payload_len);
                /* 
                 * 注意这两个free
                 * */
                free_buf(flow->packet->pool,flow->packet);
                free_buf(flow->pool,flow);
                session->cur_len += flow->payload_len;
                gettimeofday(&session->last_time, NULL);
                break;
            }
            next = next->next;
        }
        if(next == NULL)
        {
            /* Add new item */
            get_buf();
        }
        next = bucket->list;
        while(next != NULL)
        {
            /* Do check here */
            next = next->next; 
        }
    }
}
