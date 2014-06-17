/*************************************************************************
	> File Name: parser_setr.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 08 May 2014 10:50:13 AM CST
 ************************************************************************/
#ifndef parser_set_H
#define parser_set_H
typedef struct _parser
{
    pthread_t id;
    /*
     * 这个线程所处理的的包数
     * */
    uint64_t total;
    queue_t * queue;
    pool_t  * pool;
    manager_set_t * manager_set;
    struct timeval old;
    struct timeval now;
}parser_t;
typedef struct parser_m
{
    parser_t * parser;
    uint32_t numbers;
}parser_set_t;
extern parser_set_t * parser_set;
void init_parser_set(int numbers);
/*
 * 销毁线程和销毁数据分两步走
 * */
void finish_parser_set(parser_set_t * parser_set);
void destroy_parser_set(parser_set_t * parser_set);
void * packet_parser_loop(void * arg);
#endif
