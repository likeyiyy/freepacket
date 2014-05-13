/*************************************************************************
	> File Name: session_queue.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:30:41 PM CST
 ************************************************************************/
#ifndef session_queue_MANAGER_H
#define session_queue_MANAGER_H
typedef struct _session_queue
{
    int push_pos;
    int pop_pos;
    void ** node;
    int item_size;
    unsigned int total;
    unsigned int free_num;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t full;
}session_queue_t;

session_queue_t * init_session_queue(int length,int item_size);

void destroy_session_queue(session_queue_t * session_queue);

bool push_session_buf(session_queue_t * session_queue,void * data);

bool pop_session_buf(session_queue_t * session_queue,void ** data);

bool is_empty_session_queue(session_queue_t * session_queue);

bool is_full_session_queue(session_queue_t * session_queue);
#endif
