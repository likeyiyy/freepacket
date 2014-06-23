/*************************************************************************
    > File Name: parser_setr.c
    > Author: likeyi
    > Mail: likeyiyy@sina.com 
    > Created Time: Thu 08 May 2014 10:49:55 AM CST
 ************************************************************************/
#include "includes.h"

uint32_t ipfrag_hash_rnd;
/* NOTE: Arguments are modified. */
#define __jhash_mix(a, b, c) \
        { \
          a -= b; a -= c; a ^= (c>>13); \
          b -= c; b -= a; b ^= (a<<8); \
          c -= a; c -= b; c ^= (b>>13); \
          a -= b; a -= c; a ^= (c>>12); \
          b -= c; b -= a; b ^= (a<<16); \
          c -= a; c -= b; c ^= (b>>5); \
          a -= b; a -= c; a ^= (c>>3); \
          b -= c; b -= a; b ^= (a<<10); \
          c -= a; c -= b; c ^= (b>>15); \
        } 
            
            /* The golden ration: an arbitrary value */
#define JHASH_GOLDEN_RATIO 0x9e3779b9


            /* A special ultra-optimized versions that knows they are hashing exactly
            *  * 3, 2 or 1 word(s).
            *   * 
            *    * NOTE: In partilar the "c += length; __jhash_mix(a,b,c);" normally
            *     * done at the end is not done here.
            *      */ 
static inline uint32_t jhash_3words(uint32_t a, uint32_t b, uint32_t c, uint32_t initval)
{ 
                a += JHASH_GOLDEN_RATIO;
                b += JHASH_GOLDEN_RATIO;
                c += initval;
                      
                __jhash_mix(a, b, c);
                      
                return c;
}   


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
    parser->queue  = init_queue(NODE_POOL_SIZE);
    parser->pool   = init_pool(MANAGER_NODE_POOL,
                              MANAGER_QUEUE_LENGTH,
                              sizeof(flow_item_t));
    parser->pool->pool_type = 1;
    parser->total = 0;
    parser->manager_set  = manager_set;
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
    srand((unsigned int)time(NULL));
    ipfrag_hash_rnd = 0xff00ff00;
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
//static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
void * packet_parser_loop(void * arg)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    parser_t * parser = (parser_t *)arg;
    packet_t * packet;
    flow_item_t * flow = NULL;
    unsigned int index;
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
                index = hash_index(flow,parser->manager_set);
//                printf("INDEX: %u\n",index);
                push_session_buf(parser->manager_set->manager[index].queue,flow);
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
                index = hash_index(flow,parser->manager_set);
                push_session_buf(parser->manager_set->manager[index].queue,flow);
            }
        }
       else
        {
            free_packet(packet);
        }
        pthread_testcancel();
    }
}
