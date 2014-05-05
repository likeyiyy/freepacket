/*************************************************************************
	> File Name: pool_manager.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:30:41 PM CST
 ************************************************************************/

#ifndef POOL_MANAGER_H
#define POOL_MANAGER_H
typedef enum _pool_type
{
    PACKET_POOL,
    SESSION_POOL,
    BUFFER_POOL,
    MANAGER_NODE_POOL
}pool_type_t;
#if 0
typedef struct _node
{
    void * data;
}node_t;
#endif
typedef struct _pool
{
    int push_pos;
    int pop_pos;
    void ** node;
    void * buffer;
    int item_size;
    unsigned int total;
    unsigned int free_num;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
}pool_t;

extern static pool_t * packet_pool, * session_pool, * buffer_pool;
extern static pool_t * manager_node_pool;
pool_t * init_pool(pool_type_t type,int size,int item_size);
pool_t * get_pool(pool_type_t type);
bool free_buf(pool_t * pool,void * data);
bool get_buf(pool_t * pool,void ** data);
bool is_empty_pool(pool_t * pool);
bool is_full_pool(pool_t * pool);
#endif
