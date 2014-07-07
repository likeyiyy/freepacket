/*************************************************************************
    > File Name: parser_setr.c
    > Author: likeyi
    > Mail: likeyiyy@sina.com 
    > Created Time: Thu 08 May 2014 10:49:55 AM CST
 ************************************************************************/
#include "includes.h"

#define MAKE_HASH(v1,v2,h1,f1,f2,f3,f4,SIZE) \
(\
            v1 = f1 ^ f2,\
            v2 = f3 ^ f4,\
            h1 = v1 << 8,\
            h1 ^= v1 >> 4,\
            h1 ^= v1 >> 12,\
            h1 ^= v1 >> 16,\
            h1 ^= v2 << 6, \
            h1 ^= v2 << 10, \
            h1 ^= v2 << 14, \
            h1 ^= v2 >> 7,\
            h1%SIZE\
) 
static inline uint32_t make_hash(uint32_t f1,uint32_t f2,uint32_t f3,uint32_t f4,uint32_t SIZE) 
{ 
    uint32_t v1 = f1 ^ f2; 
    uint32_t v2 = f3 ^ f4; 
    uint32_t h1 = v1 << 8;
    h1 ^= v1 >> 4; 
    h1 ^= v1 >> 12; 
    h1 ^= v1 >> 16; 
                      
    h1 ^= v2 << 6; 
    h1 ^= v2 << 10; 
    h1 ^= v2 << 14; 
                      
    h1 ^= v2 >> 7; 
                      
    return h1%SIZE; 
}   

extern manager_set_t * manager_set;
parser_set_t * parser_set;
static inline unsigned int hash_index(flow_item_t * flow,manager_set_t * manager_set)
{
    uint32_t v1,v2,h1;
    return MAKE_HASH(v1,v2,h1,flow->lower_ip,flow->upper_ip,flow->lower_port,flow->upper_port,manager_set->length);
}
static inline void init_single_parser(parser_t * parser)
{
    int pool_size = global_config->parser_pool_size;
    parser->queue  = init_common_queue(PARSER_POOL_SIZE,sizeof(packet_t));
    parser->pool   = init_pool(PARSER_POOL,
                              pool_size,
                              sizeof(flow_item_t));
    parser->pool->pool_type = PARSER_POOL;
    parser->total = 0;
    parser->manager_set  = manager_set;
    parser->drop_cause_pool_empty = 0;
    parser->drop_cause_no_payload = 0;
    parser->drop_cause_unsupport_protocol = 0;
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
#if 0
    int i = 0;
    for(i = 0; i < parser_set->numbers;++i)
    {
        destroy_queue(parser_set->parser[i].queue);
    }
    free(parser_set->parser);
    parser_set->parser = NULL;
    free(parser_set);
    parser_set = NULL;
#endif
}
static inline void free_packet(packet_t * packet)
{
    free_buf(packet->pool,packet);
}
static inline void make_flow_item_tcp(flow_item_t * flow,packet_t * packet,int header_len)
{
    flow->packet   = packet;
    int eth_hdr_len = sizeof(struct ethhdr);
    struct iphdr * ip_hdr = (struct iphdr *)(packet->data + eth_hdr_len);
    int ihl = ip_hdr->ihl * 4;
    struct tcphdr * tcp_hdr = 
                        (struct tcphdr *)(packet->data +
                                         sizeof(struct ethhdr) +
                                          ihl);            
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
static inline void make_flow_item_udp(flow_item_t * flow,packet_t * packet,int header_len)
{
    flow->packet   = packet;
    int eth_hdr_len = sizeof(struct ethhdr);
    struct iphdr * ip_hdr = (struct iphdr *)(packet->data + eth_hdr_len);
    int ihl = ip_hdr->ihl * 4;
    struct udphdr * udp_hdr = 
                        (struct udphdr *)(packet->data +
                                          sizeof(struct ethhdr) +
                                          ihl);
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
static inline void free_flow(flow_item_t * flow)
{
    /* 
    * 注意这两个free
    * */
    free_buf(flow->packet->pool,flow->packet);
    free_buf(flow->pool,flow);
}

typedef void (TranHandler)(flow_item_t * flow,packet_t * packet,int header_len);
static int parser_process(parser_t * parser, 
                          int header_len, 
                          packet_t * packet,
                          TranHandler * tranhandler)
{

    unsigned int index;
    flow_item_t * flow = NULL;
    /* 说明是空负载。 */
    if(header_len >= packet->length)
    {
        free_packet(packet);
        parser->drop_cause_no_payload += packet->length;
        return -2;
    }
    else
    {
    /*
    * 从pool中取一个包头。
    * */
        if(get_buf(parser->pool,NO_WAIT_MODE,(void **)&flow) < 0)        
        {
            free_packet(packet);
            parser->drop_cause_pool_empty += packet->length;
            return -1;
        }
        flow->pool = parser->pool;
        tranhandler(flow, packet, header_len);
    /*
    * 送给下个流水线的队列。
    * */
        index = hash_index(flow,parser->manager_set);
        push_common_buf(parser->manager_set->manager[index].queue,flow);
    }
    return 0;

}
//static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
void * packet_parser_loop(void * arg)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    parser_t * parser = (parser_t *)arg;
    packet_t * packet;
    while(1)
    {
        /*
        * 从队列中取出一个数据包
        * */
        pop_common_buf(parser->queue,(void **)&packet);
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
            parser_process(parser,header_len,packet,make_flow_item_tcp);

        }
        else if(ip_hdr->protocol == IPPROTO_UDP)
        {
            int udp_len = 16;
            header_len = eth_hdr_len + ihl + udp_len;
            parser_process(parser,header_len,packet,make_flow_item_tcp);
        }
       else
        {
            free_packet(packet);
            parser->drop_cause_unsupport_protocol += packet->length;
        }
        pthread_testcancel();
    }
}
void init_parser_set(sim_config_t * config)
{
    /* 生成制定数量的线程数的结构体。
     * */
    int numbers = config->parser_nums;
    parser_set = malloc(sizeof(parser_set_t));
    exit_if_ptr_is_null(parser_set,"parser_set alloc error");
    parser_set->parser  = malloc(numbers * sizeof(parser_t));
    exit_if_ptr_is_null(parser_set->parser,"parser_set->parser alloc error");
    parser_set->numbers = numbers;
    srand((unsigned int)time(NULL));
    int i = 0;
    for(i = 0; i < numbers; i++)
    {
        init_single_parser(&parser_set->parser[i]); 
        pthread_create(&parser_set->parser[i].id,
                NULL,
                packet_parser_loop,
                &parser_set->parser[i]);
    }
}
