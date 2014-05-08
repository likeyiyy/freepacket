/*************************************************************************
	> File Name: packet_parser.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 08 May 2014 10:49:55 AM CST
 ************************************************************************/

#include "includes.h"

parser_set_t * parser_set;
static inline void init_single_parser(parser_t * parser)
{
    parser->queue = init_queue(NODE_POOL_SIZE);
    parser->total = 0;
}
void init_packet_parse(int numbers)
{
    /* 生成制定数量的线程数的结构体。
     * */
    parser_set = malloc(sizeof(parser_set_t));
    exit_if_ptr_is_null(parser_set,"parser_set alloc error");
    parser_set->parser  = malloc(numbers * sizeof(parser_t));
    exit_if_ptr_is_null(parser_set->parser,"parser_set->parser alloc error");
    parser_set->numbers = numbers;
    
    int i = 0;
    for(i = 0; i < numbers; i++)
    {
        init_single_parser(&parser_set->parser[i]); 
        pthread_create(&parser_set->parser[i].id,
                NULL,
                print_parser,
                &parser_set->parser[i]);
    }

}
parser_t * get_next_parser(parser_set_t * parser_set)
{
    if(parser_set->cur_num == parser_set->numbers)
    {
        return &parser_set->parser[parser_set->cur_num++];
    }
    else
    {
        parser_set->cur_num = 0;
        return &parser_set->parser[parser_set->cur_num];
    }
}
static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
void * print_parser(void * arg)
{
    parser_t * parser = (parser_t *)arg;
    packet_t * packet;
    while(1)
    {
        /*
        * 从队列中取出一个数据包
        * */
        int err = pop_from_queue(parser->queue,(void **)&packet);
        if(err == 0)
        {
            /*
            * 打印这个数据包。
            * */
            ++parser->total;
#if 0
            pthread_mutex_lock(&print_lock);
            parse_full_packet(packet->data); 
            print_packet(packet->data,packet->length);
            pthread_mutex_unlock(&print_lock);
#endif
        }
    }
}
