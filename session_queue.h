/*************************************************************************
	> File Name: session_queue_manager.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:30:41 PM CST
 ************************************************************************/

#ifndef session_queue_MANAGER_H
#define session_queue_MANAGER_H
typedef enum _session_queue_type
{
    PACKET_session_queue,
    SESSION_session_queue,
    BUFFER_session_queue,
    MANAGER_NODE_session_queue
}session_queue_type_t;
#if 0
typedef struct _node
{
    void * data;
}node_t;
#endif
typedef struct _session_queue
{
    int push_pos;
    int pop_pos;
    void ** node;
    void * buffer;
    int item_size;
    unsigned int total;
    unsigned int free_num;
    pthread_mutex_t mutex;
    pthread_cond_t full;
    pthread_cond_t empty;
}session_queue_t;

session_queue_t * init_session_queue(int length,int item_size);
void destroy_session_queue(session_queue_t * session_queue);
session_queue_t * get_session_queue(session_queue_type_t type);
bool push_buf(session_queue_t * session_queue,void * data);
bool pop_buf(session_queue_t * session_queue,void ** data);
bool is_empty_session_queue(session_queue_t * session_queue);
bool is_full_session_queue(session_queue_t * session_queue);
#endif
