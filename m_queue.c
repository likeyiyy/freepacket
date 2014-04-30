/*************************************************************************
	> File Name: m_queue.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Fri 25 Apr 2014 09:29:10 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "m_queue.h"
#include "includes.h"
queue_t * init_queue()
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
    return q;
}
int is_empty(queue_t * q)
{
    if(q->length == 0)
        return 1;
    return 0;
}
int push_to_queue(queue_t * q, void * data)
{
    pthread_mutex_lock(&q->lock);
    node_t * node = malloc(sizeof(node_t));
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
        FREE(node);
        node = NULL; 
        --q->length;
        err = 0;
    }
    pthread_mutex_unlock(&q->lock);
    return err;
}
