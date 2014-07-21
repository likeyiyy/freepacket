/*************************************************************************
	> File Name: common_queue.c
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
common_queue_t * init_common_queue(int numbers,int item_size)
{
    assert(numbers > 0);
    common_queue_t * common_queue = malloc(sizeof(common_queue_t));
    exit_if_ptr_is_null(common_queue,"alloc common_queue error");
    common_queue->node = malloc(numbers * sizeof(void *));
    exit_if_ptr_is_null(common_queue->node,"alloc common_queue node error");
    bzero(common_queue->node,numbers);
    common_queue->item_size = item_size;
    common_queue->total = numbers;
    common_queue->length = 0;
    common_queue->pop_pos = 0;
    /*
    * 初始化时为满的。
    * */
    common_queue->push_pos = 0;
    pthread_mutex_init(&common_queue->mutex,NULL);
    pthread_cond_init(&common_queue->empty,NULL);
    return common_queue;
}
/*
* 销毁一个缓冲区池子
* 时间：2014年5月8日17:49:21
* 作者: likeyi
* */
void destroy_common_queue(common_queue_t * common_queue)
{
    pthread_mutex_destroy(&common_queue->mutex); 
    pthread_cond_destroy(&common_queue->empty);
    free(common_queue->node);
    common_queue->node = NULL;
    free(common_queue);
    common_queue = NULL;
}
bool push_common_buf(common_queue_t * common_queue,int flag,void * data)
{
    pthread_mutex_lock(&common_queue->mutex);
    while(((common_queue->push_pos + 1) % common_queue->total == common_queue->pop_pos))
    {
        DEBUG("Error:common_queue is full\n");
        if(flag == NO_WAIT_MODE)
        {
            pthread_mutex_unlock(&common_queue->mutex);
            return false;
        }
        else
        {
            pthread_cond_wait(&common_queue->full,&common_queue->mutex);
        }
    }
    common_queue->node[common_queue->push_pos] = data;
    ++common_queue->push_pos;
    ++common_queue->length;
    if(common_queue->push_pos >= common_queue->total)
    {
        common_queue->push_pos = 0;
    }
    pthread_cond_signal(&common_queue->empty);
    pthread_mutex_unlock(&common_queue->mutex);
    return true;
}
/*
*  本函数不用is_empty_common_queue，因为防止锁中锁
* */
bool pop_common_buf(common_queue_t * common_queue,void ** data)
{
    pthread_mutex_lock(&common_queue->mutex);
    while(common_queue->push_pos == common_queue->pop_pos)
    {
        DEBUG("Error:common_queue is empty\n");
        pthread_cond_wait(&common_queue->empty,&common_queue->mutex);
    }
    *data = common_queue->node[common_queue->pop_pos];
    ++common_queue->pop_pos;
    --common_queue->length;
    if(common_queue->pop_pos >= common_queue->total)
    {
        common_queue->pop_pos = 0;
    }
    pthread_cond_signal(&common_queue->full);
    pthread_mutex_unlock(&common_queue->mutex);
    return true;
}
/*
*  
* */
bool is_empty_common_queue(common_queue_t * common_queue)
{
    bool result;
    pthread_mutex_lock(&common_queue->mutex);
    result = (common_queue->push_pos == common_queue->pop_pos);
    pthread_mutex_unlock(&common_queue->mutex);
    return result;
}
bool is_full_common_queue(common_queue_t * common_queue)
{
    bool result;
    pthread_mutex_lock(&common_queue->mutex);
    result =((common_queue->push_pos + 1) % common_queue->total == common_queue->pop_pos);
    pthread_mutex_unlock(&common_queue->mutex);
    return result;
}
