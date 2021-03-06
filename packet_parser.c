/*************************************************************************
    > File Name: parser_groupr.c
    > Author: likeyi
    > Mail: likeyiyy@sina.com 
    > Created Time: Thu 08 May 2014 10:49:55 AM CST
 ************************************************************************/
#include "includes.h"

static parser_group_t * global_parser_group = NULL;

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

static inline unsigned int hash_index(flow_item_t * flow,manager_group_t * manager_group)
{
    uint32_t v1,v2,h1;
    return MAKE_HASH(v1,v2,h1,flow->lower_ip,flow->upper_ip,flow->lower_port,flow->upper_port,manager_group->numbers);
}

static inline void init_single_parser(parser_t * parser)
{
    int pool_size = global_config->parser_pool_size;
    parser->queue  =  memalign(64,sizeof(*parser->queue));
    assert(parser->queue != NULL);
    swsr_queue_init(parser->queue);
    
	/*
	 * 初始化pool
	 * */
	pool_size = 1024;
    parser->pool   = memalign(64, sizeof(mwsr_pool_t));
	assert(parser->pool);
	mwsr_pool_init(parser->pool);
	char * buffer = malloc(pool_size * sizeof(flow_item_t));
	exit_if_ptr_is_null(buffer,"alloc pool buffer error");
	for(int j = 0; j < pool_size; ++j)
	{
		flow_item_t * flow = (flow_item_t *)(buffer + j * sizeof(flow_item_t));
		flow->pool = parser->pool;
		mwsr_pool_enqueue(parser->pool,flow);
	}

    parser->total = 0;
    parser->drop_cause_pool_empty = 0;
    parser->drop_cause_no_payload = 0;
    parser->drop_cause_unsupport_protocol = 0;
}
void finish_parser_group(parser_group_t * parser_group)
{
    int i = 0;
    for(i = 0; i < parser_group->numbers;++i)
    {
        pthread_cancel(parser_group->parser[i].id);
    }
}
void destroy_parser_group(parser_group_t * parser_group)
{
#if 0
    int i = 0;
    for(i = 0; i < parser_group->numbers;++i)
    {
        destroy_queue(parser_group->parser[i].queue);
    }
    free(parser_group->parser);
    parser_group->parser = NULL;
    free(parser_group);
    parser_group = NULL;
#endif
}
static inline void make_flow_item_tcp(flow_item_t * flow,packet_t * packet,int len)
{
    flow->packet   = packet;
    int eth_hdr_len = sizeof(struct ethhdr);
    struct iphdr * ip_hdr = (struct iphdr *)(packet->data + PAY_LEN + eth_hdr_len);
    int ihl = ip_hdr->ihl * 4;
    struct tcphdr * tcp_hdr = (struct tcphdr *)(packet->data + PAY_LEN + eth_hdr_len + ihl);            
	int tcp_len = tcp_hdr->doff * 4;
    int header_len = eth_hdr_len + ihl + tcp_len + PAY_LEN;
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
    flow->payload_len = packet->length - header_len + PAY_LEN;
}
static inline void make_flow_item_udp(flow_item_t * flow,packet_t * packet,int header_len)
{
    flow->packet   = packet;
    int eth_hdr_len = sizeof(struct ethhdr);
    struct iphdr * ip_hdr = (struct iphdr *)(packet->data + PAY_LEN + eth_hdr_len);
    int ihl = ip_hdr->ihl * 4;
    struct udphdr * udp_hdr = 
                        (struct udphdr *)(packet->data +
					  PAY_LEN +
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
    flow->payload_len = packet->length - header_len + PAY_LEN;
}

//static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
#define COUNT 64
void * packet_parser_loop(void * arg)
{
    parser_t * parser = (parser_t *)arg;
    packet_t * packet[COUNT];
    flow_item_t * flow[COUNT];
    manager_group_t * manager_group = get_manager_group();
    unsigned int index;
    while(1)
   	{
        /*
        * 从队列中取出一个数据包
        * */
		parser->alive++;
        while(unlikely(swsr_queue_dequeue_multiple(parser->queue,(void **)packet,COUNT) != 0))
		{
			continue;
		}
		if(global_config->pipe_depth > 2)
		{
			while(unlikely(mwsr_pool_dequeue_multiple(parser->pool,(void **)flow,COUNT) != 0))
			{
				continue;
			}
            /*
            * 校验数据包。
            * */
			for(int k = 0; k < COUNT; k++)
			{
				make_flow_item_tcp(flow[k],packet[k],0);
			}
            /*
            * 送给下个流水线的队列。
            * */
			if(global_config->pipe_depth > 3)
			{
				for(int k = 0; k < COUNT; k++)
				{
            		index = hash_index(flow[k],manager_group);
            		ghash_view[index]++;
					while(unlikely(mwsr_queue_enqueue(manager_group->manager[index].queue,flow[k]) != 0))
					{
						continue;	
					}
				}
			}
			else
			{
    			while(unlikely(mwsr_pool_enqueue_multiple(packet[0]->pool,(void **)packet,COUNT) != 0))
				{
						continue;	
				}
    			while(unlikely(mwsr_pool_enqueue_multiple(parser->pool,(void **)flow,COUNT) != 0))
				{
						continue;	
				}
			}
		}
	
		else
		{
    		while(unlikely(mwsr_pool_enqueue_multiple(packet[0]->pool,(void **)packet,COUNT) != 0))
			{
				continue;	
			}
		}
	}
}
parser_group_t * init_parser_group(sim_config_t * config)
{
    if(global_parser_group != NULL)
    {
        /*
        * 当global_parser_group不为NULL的时候，我们反而返回NULL
        * 是因为我们不想让别的线程使用此数据结构。
        * */
        return NULL;
    }
    /* 生成制定数量的线程数的结构体。
     * */
    int numbers = config->parser_nums;
    global_parser_group = malloc(sizeof(parser_group_t));
    exit_if_ptr_is_null(global_parser_group,"parser_group alloc error");
    global_parser_group->parser  = malloc(numbers * sizeof(parser_t));
    exit_if_ptr_is_null(global_parser_group->parser,"parser_group->parser alloc error");
    global_parser_group->numbers = numbers;
    srand((unsigned int)time(NULL));
    int i = 0;
    for(i = 0; i < numbers; i++)
    {
        init_single_parser(&global_parser_group->parser[i]); 
        pthread_create(&global_parser_group->parser[i].id,
                		NULL,
                		packet_parser_loop,
                		&global_parser_group->parser[i]);
    }
    return global_parser_group;
}
parser_group_t * get_parser_group()
{
    return global_parser_group;
}
