/*************************************************************************
	> File Name: parser_groupr.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 08 May 2014 10:50:13 AM CST
 ************************************************************************/
#ifndef parser_group_H
#define parser_group_H



typedef struct _parser
{
    pthread_t id;
    /*
     * 这个线程所处理的的包数
     * */
    uint64_t total;
    free_queue_t * queue;
<<<<<<< HEAD
    free_pool_t  * pool;
=======
    pool_t  * pool;
>>>>>>> 568c01e40f7a8bf36c59c744658dbc6f16f87f25
	int alive;
    uint64_t drop_cause_pool_empty;
    uint64_t drop_cause_no_payload;
    uint64_t drop_cause_unsupport_protocol;
}parser_t;
typedef struct parser_m
{
    parser_t * parser;
    uint32_t numbers;
}parser_group_t;

parser_group_t * get_parser_group();
parser_group_t * init_parser_group(sim_config_t * config);
/*
 * 销毁线程和销毁数据分两步走
 * */
void finish_parser_group(parser_group_t * parser_group);
void destroy_parser_group(parser_group_t * parser_group);
#endif
