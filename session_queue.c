/*************************************************************************
	> File Name: session_queue.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:30:33 PM CST
 ************************************************************************/
#include "includes.h"
/*
* Date:     2014年4月30日15:53:00
* Author:   likeyi
* Function: 初始化一个所有空闲的池子。
* Input:
* Output:
* */
session_queue_t * init_session_queue(int numbers,int item_size)
{
    assert(numbers > 0);
    session_queue_t * session_queue = malloc(sizeof(session_queue_t));
    exit_if_ptr_is_null(session_queue,"alloc session_queue error");
    session_queue->node = malloc(numbers * sizeof(void *));
    exit_if_ptr_is_null(session_queue->node,"alloc session_queue node error");
    bzero(session_queue->node,numbers);
    session_queue->item_size = item_size;
    session_queue->total = numbers;
    session_queue->free_num = numbers;
    session_queue->pop_pos = 0;
    /*
    * 初始化时为满的。
    * */
    session_queue->push_pos = 0;
    pthread_mutex_init(&session_queue->mutex,NULL);
    pthread_cond_init(&session_queue->empty,NULL);
    return session_queue;
}
/*
* 销毁一个缓冲区池子
* 时间：2014年5月8日17:49:21
* 作者: likeyi
* */
void destroy_session_queue(session_queue_t * session_queue)
{
    pthread_mutex_destroy(&session_queue->mutex); 
    pthread_cond_destroy(&session_queue->empty);
    free(session_queue->node);
    session_queue->node = NULL;
    free(session_queue);
    session_queue = NULL;
}
bool push_session_buf(session_queue_t * session_queue,void * data)
{
    pthread_mutex_lock(&session_queue->mutex);
    while(((session_queue->push_pos + 1) % session_queue->total == session_queue->pop_pos))
    {
        DEBUG("Error:session_queue is full\n");
        pthread_cond_wait(&session_queue->full,&session_queue->mutex);
    }
    session_queue->node[session_queue->push_pos] = data;
    ++session_queue->push_pos;
    --session_queue->free_num;
    if(session_queue->push_pos >= session_queue->total)
    {
        session_queue->push_pos = 0;
    }
    pthread_cond_signal(&session_queue->empty);
    pthread_mutex_unlock(&session_queue->mutex);
}
/*
*  本函数不用is_empty_session_queue，因为防止锁中锁
* */
bool pop_session_buf(session_queue_t * session_queue,void ** data)
{
    pthread_mutex_lock(&session_queue->mutex);
    while(session_queue->push_pos == session_queue->pop_pos)
    {
        DEBUG("Error:session_queue is empty\n");
        pthread_cond_wait(&session_queue->empty,&session_queue->mutex);
    }
    *data = session_queue->node[session_queue->pop_pos];
    ++session_queue->pop_pos;
    ++session_queue->free_num;
    if(session_queue->pop_pos >= session_queue->total)
    {
        session_queue->pop_pos = 0;
    }
    pthread_cond_signal(&session_queue->full);
    pthread_mutex_unlock(&session_queue->mutex);
}
/*
*  
* */
bool is_empty_session_queue(session_queue_t * session_queue)
{
    bool result;
    pthread_mutex_lock(&session_queue->mutex);
    result = (session_queue->push_pos == session_queue->pop_pos);
    pthread_mutex_unlock(&session_queue->mutex);
    return result;
}
bool is_full_session_queue(session_queue_t * session_queue)
{
    bool result;
    pthread_mutex_lock(&session_queue->mutex);
    result =((session_queue->push_pos + 1) % session_queue->total == session_queue->pop_pos);
    pthread_mutex_unlock(&session_queue->mutex);
    return result;
}
