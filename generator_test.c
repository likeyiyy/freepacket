/*************************************************************************
	> File Name: generator_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:40 PM CST
 ************************************************************************/

#include "includes.h"
extern generator_set_t * generator_set;
extern parser_set_t * parser_set;
extern config_t * config;
uint64_t send_count = 0;
uint64_t recv_count = 0;
void sig_statistics(int arg)
{
    uint64_t sum = 0;
    uint64_t total_send = 0;
    int i = 0;
    for(i = 0; i < generator_set->numbers; ++i)
    {
        total_send += generator_set->generator[i].total_send_byte;
        pthread_cancel(generator_set->generator[i].id);

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
        send_count += generator_set->generator[k].total_send_byte;
    }
    /*
    * 消费者统计
    * */
    for(k = 0; k < i; k++)
    {
		printf("%d thread: queue length:%d\n",k,parser_set->parser[k].queue->length);
        recv_count += parser_set->parser[k].total;
    }
}
int main(int argc,char ** argv)
{
    int i,j;
    assert(argc == 3);
    int parser_nums = atoi(argv[1]);
    int gener_nums  = atoi(argv[2]);
    //for(i = 1; i < 2; ++i)
    {
        //for(j = 4; j < 5; ++j)
        {
            /*
            * 初始化线程
            * */

            init_parser_set(parser_nums);

            init_generator_set(gener_nums);
            /*
            * 运行5秒
            * */
            sleep(5);

            /*
            * 取消掉所有线程。
            * */
            finish_generator(generator_set);
            //printf("queue length:%d\n",parser_set->parser[i].queue->length);
            finish_parser_set(parser_set);
            /*
            * 统计结果
            * */
            count_now(parser_nums,gener_nums);
            printf("send:%d count:%llu\t",gener_nums,send_count / (5 * 1024 * 1024));
            printf("recv:%d count:%llu\n",parser_nums,recv_count / (5 * 1024 * 1024));

            /*
            * 删除线程的数据结构
            * */
            //destroy_generator(generator_set);
            //destroy_packet_parse(parser_set);

        }

    }
    /*
    * 初始化几个线程，packet_parser。
    * */
    /*
    * 初始化产生数据包的线程。
    * */
    exit(0);
    pthread_exit(NULL);
}

