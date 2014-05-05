/*************************************************************************
	> File Name: pool_manager.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:30:33 PM CST
 ************************************************************************/

#include "includes.h"
static pool_t * packet_pool, * session_pool, * buffer_pool;
static pool_t * manager_node_pool;
static inline void exit_if_ptr_is_null(void * ptr,const char * message) 
{
    if(ptr == NULL)
    {   
        printf("%s\n",message);
        exit(-1);
    }   
}

/*
* Date:     2014年4月30日15:53:00
* Author:   likeyi
* Function: 初始化一个所有空闲的池子。
* Input:
* Output:
* */
pool_t * init_pool(pool_type_t type,int size,int item_size)
{
    assert(size > 0);
    pool_t * pool = malloc(sizeof(pool_t));
    exit_if_ptr_is_null(pool,"alloc pool error");
    
    pool->node = malloc(size * sizeof(void *));
    exit_if_ptr_is_null(pool->node,"alloc pool node error");

    pool->buffer = malloc(size * item_size);
    exit_if_ptr_is_null(pool->buffer,"alloc pool buffer error");

    int i = 0;

    for(i = 0; i < size; ++i)
    {
        /*
        * 这个复杂的复制时为了，让node_t[]数组里面的指针指向真实的buffer.
        * */
        pool->node[i] = pool->buffer + i * item_size;
    }
    pool->item_size = item_size;
    pool->total = size;
    pool->free_num = size;
    pool->pop_pos = 0;
    /*
    * 初始化时为满的。
    * */
    pool->push_pos = size - 1;
    pthread_mutex_init(&pool->mutex,NULL);
    pthread_cond_init(&pool->empty,NULL);
    switch(type)
    {
        case PACKET_POOL:
        packet_pool = pool;
        break;

        case SESSION_POOL:
        session_pool = pool;
        break;

        case BUFFER_POOL:
        buffer_pool = pool;
        break;

        case MANAGER_NODE_POOL:
        manager_node_pool = pool;
        break;
    }
    return pool;
}
pool_t * get_pool(pool_type_t type)
{
    switch(type)
    {
        case PACKET_POOL:
        return packet_pool;
        break;

        case SESSION_POOL:
        return session_pool;
        break;

        case BUFFER_POOL:
        return buffer_pool;
        break;

        case MANAGER_NODE_POOL:
        return manager_node_pool;
        break;
    }
}
bool free_buf(pool_t * pool,void * data)
{
    pthread_mutex_lock(&pool->mutex);
    /*
    * 我们不测试pool是否已经满了。
    * 假如pool已经满了，现在仍然往pool写入
    * 那将是不可思议的事情。
    * */
    pool->node[pool->push_pos] = data;
    ++pool->push_pos;
    ++pool->free_num;
    if(pool->push_pos >= pool->total)
    {
        pool->push_pos = 0;
    }
    pthread_cond_signal(&pool->empty);
    pthread_mutex_unlock(&pool->mutex);
}
/*
*  本函数不用is_empty_pool，因为防止锁中锁
* */
bool get_buf(pool_t * pool,void ** data)
{
    pthread_mutex_lock(&pool->mutex);

    while(pool->push_pos == pool->pop_pos)
    {
        printf("Error:pool is empty\n");
        pthread_cond_wait(&pool->empty,&pool->mutex);
    }
    *data = pool->node[pool->pop_pos];
    ++pool->pop_pos;
    --pool->free_num;
    if(pool->pop_pos >= pool->total)
    {
        pool->pop_pos = 0;
    }
    pthread_mutex_unlock(&pool->mutex);
}
/*
*  
* */
bool is_empty_pool(pool_t * pool)
{
    bool result;
    pthread_mutex_lock(&pool->mutex);
    result = (pool->push_pos == pool->pop_pos);
    pthread_mutex_unlock(&pool->mutex);
    return result;
}
bool is_full_pool(pool_t * pool)
{
    bool result;
    pthread_mutex_lock(&pool->mutex);
    result =((pool->push_pos + 1) % pool->total == pool->pop_pos);
    pthread_mutex_unlock(&pool->mutex);
    return result;

}