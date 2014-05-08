/*************************************************************************
	> File Name: generator_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:40 PM CST
 ************************************************************************/

#include "includes.h"
extern generator_info_t generator_info;
extern parser_set_t * parser_set;
extern config_t * config;
uint64_t send_count[100][100];
uint64_t recv_count[100][100];
void sig_statistics(int arg)
{
    uint64_t sum = 0;
    uint64_t total_send = 0;
    int i = 0;
    for(i = 0; i < generator_info.numbers; ++i)
    {
        total_send += generator_info.generator[i].total_send_byte;
        pthread_cancel(generator_info.generator[i].id);

    }
    /*
    * 我睡十秒还不行吗？啊？
    * */
    //sleep(10);
    printf("+++++++++%d\n",parser_set->numbers);
    for(i = 0; i < parser_set->numbers; ++i)
    {
#if 1
        {
            printf("----%d parser length :%lu\n",i,parser_set->parser[i].queue->length);
        }
#endif
        sum += parser_set->parser[i].total;
    }
    sum = sum * config->pktlen;
    printf("total send:%llu total Mytes %llu\n",total_send,sum / (1024 * 1024));
    exit(0);
}
int main()
{
    int i,j;
    for(i = 1; i <= 100; ++i)
    {
        for(j = 1; j <= 100; ++j)
        {
            /*
            * 初始化线程
            * */

            init_packet_parse(i);
            init_generator(j);
            /*
            * 运行5秒
            * */
            sleep(5);

            /*
            * 取消掉所有数据，给出统计结果
            * */

            

        }

    }
    /*
    * 初始化几个线程，packet_parser。
    * */
    init_packet_parse(PACKET_PARSER_NUMS);

    /*
    * 初始化产生数据包的线程。
    * */
    init_generator(PACKET_GENERATOR_NUMS);

    signal(SIGALRM,sig_statistics);

    alarm(1);

    while(1)
    {
        sleep(10);
    }
    pthread_exit(NULL);
}

