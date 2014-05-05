/*************************************************************************
	> File Name: manager_buffer.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
    > Created Time: 2014年5月5日10:57:18
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "includes.h"
queue_t * init_queue(unsigned int node_pool_size)
{
    queue_t * q = malloc(sizeof(queue_t));
    if(q == NULL)
    {
        DEBUG("alloc queue error");
        return NULL;
    }
    q->head = q->tail = NULL;
    q->length = 0;
    int status = pthread_mutex_init(&q->lock,NULL);
    if(status != 0)
    {
        DEBUG("init mutex error");
        return NULL;
    }
    /*
     * if init pool is error,should be exit.
     * */
    q->node_pool = init_pool(MANAGER_NODE_POOL,node_pool_size,sizeof(node_t));
    return q;
}
int is_empty(queue_t * q)
{
    pthread_mutex_lock(&q->lock);
    if(q->length == 0)
    {
        pthread_mutex_unlock(&q->lock);
        return 1;
    }
    pthread_mutex_unlock(&q->lock);
    return 0;
}
int push_to_queue(queue_t * q, void * data)
{
    pthread_mutex_lock(&q->lock);
    node_t * node = NULL;
    get_buf(q->node_pool,(void **)&node);
    if(node == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return -1;
    }
    node->data = data;
    node->prev = node->next = NULL;
    if((q->head == NULL) && (q->tail == NULL))
    {
        q->head = q->tail = node; 
    }
    else
    {
        q->tail->next = node;
        node->prev = q->tail;
        q->tail = node;
    }
    ++q->length;
    pthread_mutex_unlock(&q->lock);
    return 0;

}
int pop_from_queue(queue_t * q,void ** data)
{
    pthread_mutex_lock(&q->lock);
    int err = -1;
    if(q->length != 0)
    {
            
        node_t * node = q->head;
        q->head = node->next;
        if(q->head == NULL)
        {
            q->tail = NULL;
        }
        else
        {
            q->head->prev = NULL;
        }
        *data = node->data;
    /*
     * 数据并不释放，由上层调用，由上层释放。
     * */
        free_buf(q->node_pool,node);
        node = NULL; 
        --q->length;
        err = 0;
    }
    pthread_mutex_unlock(&q->lock);
    return err;
}