/*************************************************************************
	> File Name: common_queue.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:30:41 PM CST
 ************************************************************************/
#ifndef common_queue_MANAGER_H
#define common_queue_MANAGER_H
typedef struct _common_queue
{
    int push_pos;
    int pop_pos;
    void ** node;
    int item_size;
    unsigned int total;
    unsigned int length;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t full;
}common_queue_t;
/*
* This is just a queue not a pool
* */
common_queue_t * init_common_queue(int length,int item_size);

void destroy_common_queue(common_queue_t * common_queue);

bool push_common_buf(common_queue_t * common_queue,void * data);

bool pop_common_buf(common_queue_t * common_queue,void ** data);

bool is_empty_common_queue(common_queue_t * common_queue);

bool is_full_common_queue(common_queue_t * common_queue);
#endif
