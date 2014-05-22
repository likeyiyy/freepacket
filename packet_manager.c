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
        set->manager[i].list  = NULL;
        set->manager[i].session_pool = init_pool(SESSION_POOL,
                                                SESSION_POOL_LENGTH,
                                                sizeof(session_t));
        set->manager[i].session_pool->pool_type = 2;
        set->manager[i].index = i;
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
static inline int compare_flow(session_t * list , flow_item_t * flow)
{
    session_item_t * session = &list->item;
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
/*
* 真正的工作者。
* */
void * session_worker(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    flow_item_t * flow;
    session_t * head, * next, * prev;
    while(1)
    {
        /*
         * 只有确实有数据时才返回。
         * */
        pop_session_buf(manager->queue,(void **)&flow);
        prev = next = manager->list;
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
            get_buf(manager->session_pool,(void **)&new_session);
            new_session->next = NULL;
            new_session->prev = prev;
            make_new_session(new_session,flow);
            free_flow(flow);
            /*
            * 这说明是头结点。头结点，next也是NULL.
            * */
            if(prev == NULL)
            {
                manager->list = new_session;
            }
        }
        struct timeval current_time;
        session_t * current;
        current = manager->list;
        while(current != NULL)
        {
            gettimeofday(&current_time,NULL);
            if((1.0 * current->item.cur_len > MAX_FACTOR * current->item.length) ||
                (current_time.tv_sec - current->item.last_time.tv_sec > DESTORY_TIME))
            {
                if(current->prev == NULL)
                {
                    manager->list = current->next;
                    if(current->next)
                    {
                        current->next->prev = current->prev;
                    }
                    free_buf(manager->session_pool,(void *)current); 
                }
                else
                {
                    current->prev->next = current->next;
                    if(current->next)
                    {
                        current->next->prev = current->prev;
                    }
                    free_buf(manager->session_pool,(void *)current); 
                }
            }
            /* Do check here */
            current = current->next;
        }
    }
}
