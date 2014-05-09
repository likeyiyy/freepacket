/*************************************************************************
	> File Name: packet_parser.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 08 May 2014 10:49:55 AM CST
 ************************************************************************/

#include "includes.h"

parser_set_t * parser_set;
extern pool_t * packet_pool;
static inline void init_single_parser(parser_t * parser)
{
    parser->queue = init_queue(NODE_POOL_SIZE);
    parser->packet = malloc(sizeof(packet_t));
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

void finish_packet_parse(parser_set_t * parser_set)
{
    int i = 0;
    for(i = 0; i < parser_set->numbers;++i)
    {
        pthread_cancel(parser_set->parser[i].id);
    }
}
void destroy_packet_parse(parser_set_t * parser_set)
{
    int i = 0;
    for(i = 0; i < parser_set->numbers;++i)
    {
        destroy_queue(parser_set->parser[i].queue);
    }
    free(parser_set->parser);
    parser_set->parser = NULL;
    free(parser_set);
    parser_set = NULL;
}
static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
void * print_parser(void * arg)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);

    parser_t * parser = (parser_t *)arg;
    while(1)
    {
        gettimeofday(&parser->old,NULL);
        /*
        * 从队列中取出一个数据包
        * */
        int err = pop_from_queue(parser->queue,(void **)&parser->packet);
            /*
            * 打印这个数据包。
            * */
        parser->total += parser->packet->length;
        free_buf(packet_pool,parser->packet);
        gettimeofday(&parser->now,NULL);
        //printf("-----------parser------period time:%llu\n",(parser->now.tv_usec + 1000000 - parser->old.tv_usec)%1000000) ;
        pthread_testcancel();
#if 0
            pthread_mutex_lock(&print_lock);
            parse_full_packet(packet->data); 
            print_packet(packet->data,packet->length);
            pthread_mutex_unlock(&print_lock);
#endif
    }
}
