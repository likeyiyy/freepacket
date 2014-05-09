/*************************************************************************
	> File Name: packet_parser.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 08 May 2014 10:50:13 AM CST
 ************************************************************************/

#ifndef PACKET_PARSE_H
#define PACKET_PARSE_H
typedef struct _parser
{
    pthread_t id;
    /*
     * 这个线程所处理的的包数
     * */
    uint64_t total;
    queue_t * queue;
    packet_t * packet;
    struct timeval old;
    struct timeval now;
}parser_t;

typedef struct parser_m
{
    parser_t * parser;
    int numbers;
}parser_set_t;

extern parser_set_t * parser_set;

void init_packet_parse(int numbers);
/*
 * 销毁线程和销毁数据分两步走
 * */
void finish_packet_parse(parser_set_t * parser_set);
void destroy_packet_parse(parser_set_t * parser_set);
void * print_parser(void * arg);
#endif
