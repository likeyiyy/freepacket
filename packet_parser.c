/*************************************************************************
	> File Name: parser_setr.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 08 May 2014 10:49:55 AM CST
 ************************************************************************/
#include "includes.h"

#define HASH_INDEX(flow,a) ((flow->upper_ip^flow->lower_ip^flow->upper_port^flow->lower_port^(flow->protocol>>1)&((a)->length - 1)))
extern session_set_t * session_set;
parser_set_t * parser_set;
static inline void init_single_parser(parser_t * parser)
{
    parser->queue  = init_queue(NODE_POOL_SIZE);
    parser->pool   = init_pool(MANAGER_NODE_POOL,
                              SESSION_QUEUE_LENGTH,
                              sizeof(flow_item_t));
    parser->pool->pool_type = 1;
    parser->total = 0;
    parser->session_set  = session_set;
}
void init_parser_set(int numbers)
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
void finish_parser_set(parser_set_t * parser_set)
{
    int i = 0;
    for(i = 0; i < parser_set->numbers;++i)
    {
        pthread_cancel(parser_set->parser[i].id);
    }
}
void destroy_parser_set(parser_set_t * parser_set)
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
static inline void free_packet(packet_t * packet)
{
    free_buf(packet->pool,packet);
}
static inline void make_flow_item_tcp(flow_item_t * flow,packet_t * packet,struct iphdr *ip_hdr, struct tcphdr * tcp_hdr,int header_len)
{
    flow->packet   = packet;
    if(ip_hdr->saddr >= ip_hdr->daddr)
    {
        flow->upper_ip = ip_hdr->saddr;
        flow->lower_ip = ip_hdr->daddr;
    }
    else
    {
        flow->upper_ip = ip_hdr->daddr;
        flow->lower_ip = ip_hdr->saddr;
    }
    if(tcp_hdr->source >= tcp_hdr->dest)
    {
        flow->upper_port = tcp_hdr->source;
        flow->lower_port = tcp_hdr->dest;
    }
    else
    {
        flow->upper_port = tcp_hdr->dest;
        flow->lower_port = tcp_hdr->source;
    }
    flow->protocol = ip_hdr->protocol;
    flow->payload  = packet->data + header_len;
    flow->payload_len = packet->length - header_len;
}
static inline void make_flow_item_udp(flow_item_t * flow,packet_t * packet,struct iphdr *ip_hdr,struct udphdr * udp_hdr,int header_len)
{
    flow->packet   = packet;
    if(ip_hdr->saddr >= ip_hdr->daddr)
    {
        flow->upper_ip = ip_hdr->saddr;
        flow->lower_ip = ip_hdr->daddr;
    }
    else
    {
        flow->upper_ip = ip_hdr->daddr;
        flow->lower_ip = ip_hdr->saddr;
    }
    if(udp_hdr->source >= udp_hdr->dest)
    {
        flow->upper_port = udp_hdr->source;
        flow->lower_port = udp_hdr->dest;
    }
    else
    {
        flow->upper_port = udp_hdr->dest;
        flow->lower_port = udp_hdr->source;
    }
    flow->protocol = ip_hdr->protocol;
    flow->payload  = packet->data + header_len;
    flow->payload_len = packet->length - header_len;
}
static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
void * print_parser(void * arg)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    parser_t * parser = (parser_t *)arg;
    packet_t * packet;
    flow_item_t * flow = NULL;
    int next_thread_id = 0; 
    while(1)
    {
        /*
        * 从队列中取出一个数据包
        * */
        pop_from_queue(parser->queue,(void **)&packet);
        parser->total += packet->length;
        /*
        * 校验数据包。
        * */
        int header_len = 0;
        int eth_hdr_len = sizeof(struct ethhdr);
        struct iphdr * ip_hdr = (struct iphdr *)(packet->data + eth_hdr_len);
        int ihl = ip_hdr->ihl * 4;
        if(ip_hdr->protocol == IPPROTO_TCP)
        {
            struct tcphdr * tcp_hdr = 
                        (struct tcphdr *)(packet->data +
                                         sizeof(struct ethhdr) +
                                          ihl);            
            int tcp_len = tcp_hdr->doff * 4;
            header_len = eth_hdr_len + ihl + tcp_len;
            /* 说明是空负载。 */
            if(header_len >= packet->length)
            {
                free_packet(packet);
            }
            else
            {
                /*
                * 从pool中取一个包头。
                * */
                get_buf(parser->pool,(void **)&flow);        
                flow->pool = parser->pool;
                make_flow_item_tcp(flow,packet,ip_hdr,tcp_hdr,header_len);
                /*
                * 送给下个流水线的队列。
                * */
                int index = HASH_INDEX(flow,parser->session_set);
                push_session_buf(parser->session_set->bucket[index].queue,flow);
            }
        }
        else if(ip_hdr->protocol == IPPROTO_UDP)
        {
            struct udphdr * udp_hdr = 
                        (struct udphdr *)(packet->data +
                                          sizeof(struct ethhdr) +
                                          ihl);
            int udp_len = 16;
            header_len = eth_hdr_len + ihl + udp_len;
            /* 说明是空负载。 */
            if(header_len >= packet->length)
            {
                free_packet(packet);
            }
            else
            {
                /*
                * 从pool中取一个包头。
                * */
                get_buf(parser->pool,(void **)&flow);        
                flow->pool = parser->pool;
                make_flow_item_udp(flow,packet,ip_hdr,udp_hdr,header_len);
                /*
                * 送给下个流水线的队列。
                * */
                int index = HASH_INDEX(flow,parser->session_set);
                push_session_buf(parser->session_set->bucket[index].queue,flow);
            }
        }
       else
        {
            free_packet(packet);
        }
        pthread_testcancel();
    }
}
