/*************************************************************************
	> File Name: generator_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:40 PM CST
 ************************************************************************/

#include "includes.h"
extern generator_info_t * generator_info;
extern parser_set_t * parser_set;
extern config_t * config;
uint64_t send_count[101][101] = {0};
uint64_t recv_count[101][101] = {0};
void sig_statistics(int arg)
{
    uint64_t sum = 0;
    uint64_t total_send = 0;
    int i = 0;
    for(i = 0; i < generator_info->numbers; ++i)
    {
        total_send += generator_info->generator[i].total_send_byte;
        pthread_cancel(generator_info->generator[i].id);

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

void count_now(int i,int j)
{
    /*
    * 总而言之，意思是生产者为j个，消费者为i个的时候。
    * */
    int k = 0;
    /*
    * 生产者统计。
    * */
    for(k = 0; k < j; k++)
    {
        send_count[i][j] += generator_info->generator[i].total_send_byte;
    }
    /*
    * 消费者统计
    * */
    for(k = 0; k < i; k++)
    {
        recv_count[i][j] += parser_set->parser[i].total;
    }
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
            sleep(1);

            /*
            * 取消掉所有线程。
            * */
            finish_generator(generator_info);
            finish_packet_parse(parser_set);
            /*
            * 统计结果
            * */
            count_now(i,j);
            printf("send[%d][%d]:%llu\t",i,j,send_count[i][j]);
            printf("recv[%d][%d]:%llu\n",i,j,recv_count[i][j]);

            /*
            * 删除线程的数据结构
            * */
            destroy_generator(generator_info);
            destroy_packet_parse(parser_set);

        }

    }
    /*
    * 初始化几个线程，packet_parser。
    * */
    /*
    * 初始化产生数据包的线程。
    * */
    for(i = 1; i <= 100; ++i)
    {
        for(j = 1; j <= 100; ++j)
        {
            printf("send[%d][%d]:%llu\t",send_count[i][j]);
            printf("recv[%d][%d]:%llu\t",recv_count[i][j]);
        }
    }
    pthread_exit(NULL);
}

