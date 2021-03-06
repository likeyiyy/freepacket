                    /*************************************************************************
	> File Name: packet_generator.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:16 PM CST
 ************************************************************************/
#include "includes.h"
//#define memcpy(a,b,c) do { memcpy(a,b,c);printf("packet_generator_loop memcpy here\n"); } while(0)


static generator_group_t * generator_group = NULL;
static mpipe_common_t * global_mpipe = NULL;
static pthread_mutex_t global_create_generator_lock = PTHREAD_MUTEX_INITIALIZER;
void   destroy_generator(generator_group_t * generator_group)
{
    assert(generator_group);
    int i = 0;
    for(i = 0; i < generator_group->numbers; ++i)
    {
        generator_group->generator[i].config     = NULL;
        generator_group->generator[i].pool       = NULL;
    }
    free(generator_group->generator);
    generator_group->generator = NULL;
    /* 销毁缓冲区池子 */
    /*
     * 销毁配置文件分配的内存。
     * */
    free(generator_group->config->pkt_data);
    generator_group->config->pkt_data = NULL;
    free(generator_group->config);
    generator_group->config = NULL;
    free(generator_group);
    generator_group = NULL;
}
void   finish_generator(generator_group_t * generator_group)
{
    int i = 0;
    for(i = 0; i < generator_group->numbers; ++i)
    {
        pthread_cancel(generator_group->generator[i].id);
    }
}
int pop_payload(void * payload, char * data,sim_config_t * config)
{
    int i = strlen((const char *)data);
    int j = config->pktlen - 54;
    if(i > j)
    {
        memcpy(payload,data,j);
    }
    int start = 0;
    while(j > i)
    {
        memcpy(payload + start,data,i);
        start += i;
        j -= i;
    }
    memcpy(payload + start,data,j);
    /*
    * 返回payload的长度。
    * */
    return config->pktlen - 54;
}
#define GET_NEXT_SRCIP(config) \
        ((config->saddr_cur == config->saddr_max) ? \
         (config->saddr_cur = config->saddr_min) : \
         (++config->saddr_cur))
static inline uint32_t get_next_srcip(sim_config_t * config)
{
    return config->saddr_cur == config->saddr_max ?
    config->saddr_cur = config->saddr_min:
    ++config->saddr_cur;
}
#define GET_NEXT_DSTIP(config) \
        ((config->daddr_cur == config->daddr_max) ? \
         (config->daddr_cur = config->daddr_min) : \
         (++config->daddr_cur))
static inline uint32_t get_next_dstip(sim_config_t * config)
{
    return config->daddr_cur == config->daddr_max ?
    config->daddr_cur = config->daddr_min:
    ++config->daddr_cur;
}
#define GET_NEXT_SRCPORT(config) \
        ((config->sport_cur == config->sport_max) ? \
         (config->sport_cur = config->sport_min) : \
         (++config->sport_cur))
static inline uint16_t get_next_srcport(sim_config_t * config)
{
    return config->sport_cur == config->sport_max ?
    config->sport_cur = config->sport_min:
    ++config->sport_cur;
}
#define GET_NEXT_DSTPORT(config) \
        ((config->dport_cur == config->dport_max) ? \
         (config->dport_cur = config->dport_min) : \
         (++config->dport_cur))
static inline uint16_t get_next_dstport(sim_config_t * config)
{
    return config->dport_cur == config->dport_max ?
    config->dport_cur = config->dport_min:
    ++config->dport_cur;
}
static inline int pop_transmission_tcp(void * tcph,sim_config_t * config)
{
    struct tcphdr * tcp = (struct tcphdr *)tcph;
    tcp->source = htons(GET_NEXT_SRCPORT(config));
    tcp->dest   = htons(GET_NEXT_DSTPORT(config));
    tcp->doff  = sizeof(struct tcphdr) / 4;
    //tcp->check = 0;
    //return config->pktlen - TCP_IDX;
    return 0;
}
static inline int pop_transmission_udp(void * udph,sim_config_t * config)
{
    struct udphdr * udp = (struct udphdr *)udph;
    udp->source = htons(GET_NEXT_SRCPORT(config));
    udp->dest   = htons(GET_NEXT_DSTPORT(config));
    udp->len    = htons(config->pktlen - UDP_IDX);
    //udp->check  = 0;
    //return config->pktlen - UDP_IDX;
    return 0;
}
static inline void pop_iplayer_tcp(void * iph,sim_config_t * config)
{
    struct iphdr * ip = (struct iphdr *)iph;
    ip->version = IPVERSION;
    ip->ihl     = sizeof(struct iphdr) / sizeof(uint32_t);
    ip->tot_len = htons(config->pktlen-IP_IDX);
    ip->ttl     = IPDEFTTL;
    ip->protocol = IPPROTO_TCP;
    ip->saddr   = htonl(GET_NEXT_SRCIP(config));
    ip->daddr   = htonl(GET_NEXT_DSTIP(config));
    //ip->check   = ~ip_xsum((uint16_t *)ip,sizeof(struct iphdr)/2,0);
    /*
    * Do TCP header Check Sum
    * */
    //struct tcphdr * tcp = (struct tcphdr *)((unsigned char *)ip+20);
    //uint16_t sum = 0x6 + config->pktlen - 34;
    //tcp->check = (~ip_xsum((uint16_t *)&ip->saddr,(config->pktlen-26)/2,htons(sum)));
}
static inline void pop_iplayer_udp(void * iph,sim_config_t * config)
{
    struct iphdr * ip = (struct iphdr *)iph;
    //ip->version = IPVERSION;
    ip->ihl     = sizeof(struct iphdr) / sizeof(uint32_t);
    ip->tot_len = htons(config->pktlen-IP_IDX);
    //ip->ttl     = IPDEFTTL;
    ip->protocol = IPPROTO_UDP;
    ip->saddr   = htonl(GET_NEXT_SRCIP(config));
    ip->daddr   = htonl(GET_NEXT_DSTIP(config));
    //ip->check   = ~ip_xsum((uint16_t *)ip,sizeof(struct iphdr)/2,0);
    /*
    * Do UDP header Check Sum
    * */
    //struct udphdr * udp = (struct udphdr *)((unsigned char *)ip+20);
    //uint16_t sum = 0x17 + config->pktlen - 34;
    //udp->check = (~ip_xsum((uint16_t *)((unsigned char *)ip+12),(config->pktlen-26)/2,htons(sum)));
}
static inline void pop_datalink(void * packet,sim_config_t * config)
{
    struct ethhdr * eth_hdr = (struct ethhdr *)(packet);
    memcpy(eth_hdr->h_dest,config->dstmac,ETH_ALEN);
    memcpy(eth_hdr->h_source,config->srcmac,ETH_ALEN);
    eth_hdr->h_proto = htons(ETH_P_IP);
}
typedef void (GenerHandler) (packet_t * packet,sim_config_t * config);
static inline void generator_tcp_packet(packet_t * packet,sim_config_t * config)
{
    pop_payload(packet->data+56,config->pkt_data,config);
    pop_transmission_tcp(packet->data + TCP_IDX,config);
    pop_iplayer_tcp(packet->data + IP_IDX,config);
    pop_datalink(packet->data + PAY_LEN,config);
}

void make_clean_packet(generator_t * generator)
{
    packet_t * packet;
    for(int i = 0; i < (1 << LG2_CAPACITY); i++)
    {           
            /*  
 			** 1. get buffer from pool
 			** */
        	if(likely(mwsr_pool_dequeue(generator->pool,(void **)&packet) ==  0))
        	{
            	packet->data   = (unsigned char *)packet + sizeof(packet_t);
    			while(unlikely(mwsr_pool_enqueue(generator->pool,packet) != 0))
				{
					continue;	
				}
        	}
    }           

}
static inline void generator_udp_packet(packet_t * packet,sim_config_t * config)
{
    pop_payload(packet->data+44,config->pkt_data,config);
    pop_transmission_udp(packet->data + UDP_IDX,config);
    pop_iplayer_udp(packet->data + IP_IDX,config);
    pop_datalink(packet->data + PAY_LEN,config);
}

static void make_all_packet(generator_t * generator,GenerHandler * Handler)
{               
    sim_config_t * config = generator->config;
    packet_t * packet;
    for(int i = 0; i < (1 << LG2_CAPACITY); i++)
    {           
            /*  
 			** 1. get buffer from pool
 			** */
        	if(likely(mwsr_pool_dequeue(generator->pool,(void **)&packet) ==  0))
        	{
            	packet->pool   = generator->pool;
            	packet->length = config->pktlen;
            	packet->data   = (unsigned char *)packet + sizeof(packet_t);
            /*  
 			** 2. 根据配置文件比如UDP，TCP来产生包结构。
 			***/
            	Handler(packet,config);

    			while(unlikely(mwsr_pool_enqueue(generator->pool,packet) != 0))
				{
					continue;	
				}
        	}
    }           
}  
static void packet_generator(generator_t * generator,int data_len,GenerHandler * Handler)
{
    packet_t * packet;
    uint64_t old,new;
    old = new = 0;
    sim_config_t * config = generator->config;
	int g_nums = config->generator_nums;
	int p_nums = config->parser_nums;
	int next_thread_id = generator->index;
    parser_group_t * parser_group = get_parser_group();
    if(parser_group == NULL)
    {
        printf("parser_group is null,exit now\n");
        exit(0);
    }
	make_all_packet(generator,Handler);
	int times = 500000;
    while(times)
    {
		if(global_config -> speed_mode == 1)
		{
        	old = GET_CYCLE_COUNT();
		}
        /*
        * 1. get buffer from pool
        * */
        while(unlikely(mwsr_pool_dequeue(generator->pool,(void **)&packet) !=  0))
        {
			continue;
        }
        /*
        * 2. 根据配置文件比如UDP，TCP来产生包结构。
        * */
    	struct tcphdr * tcp = (struct tcphdr *)(packet->data + TCP_IDX);
    	tcp->source = htons(GET_NEXT_SRCPORT(config));
    	tcp->dest   = htons(GET_NEXT_DSTPORT(config));
    	*(uint32_t *)(packet->data + 28) = htonl(GET_NEXT_SRCIP(config));
    	*(uint32_t *)(packet->data + 32) = htonl(GET_NEXT_DSTIP(config));
        /*
        * 3. 数据放到下一步的队列里。
        * */
        /* 数据包均匀 分部到 下一个工作的线程里。*/
		if(global_config -> pipe_depth  > 1)
		{
        	parser_t * parser = &parser_group->parser[next_thread_id];
			next_thread_id = (next_thread_id + g_nums < p_nums) ? (next_thread_id + g_nums) : generator->index;
			while(unlikely(swsr_queue_enqueue(parser->queue,packet) != 0))
			{
				continue;	
			}
		}
		else
		{
    		while(unlikely(mwsr_pool_enqueue(packet->pool,packet) != 0))
			{
		        generator->alive++;
				continue;	
			}
		}
        generator->total_send_byte += config->pktlen;
        global_loss->send_total    += config->pktlen;
		generator->alive++;
        /*4. 延时统计函数 */
  
		if(global_config -> speed_mode == 1)
		{
			new = GET_CYCLE_COUNT() - old;
        	while((int64_t)new - (int64_t)generator->config->period < 0)
        	{ 
            	new = GET_CYCLE_COUNT() - old;
        	} 
		}
    }
}

#ifdef TILERA_PLATFORM
void tilera_packet_collector(generator_t * generator)
{
    packet_t * packet;
    mpipe_common_t * mpipe = generator->mpipe;

    gxio_mpipe_iqueue_t * iqueue = mpipe->iqueues[generator->rank];
    sim_config_t * config = generator->config;
    gxio_mpipe_idesc_t * idesc;

	int g_nums = config->generator_nums;
	int p_nums = config->parser_nums;
	int next_thread_id = generator->index;
    parser_group_t * parser_group = get_parser_group();
    if(parser_group == NULL)
    {
        printf("parser_group is null,exit now\n");
        exit(0);
    }
    make_clean_packet(generator);
    while(1)
    {
		if(gxio_mpipe_iqueue_try_peek(iqueue,&idesc) > 0)
        {
            if (gxio_mpipe_iqueue_drop_if_bad(iqueue, idesc))
                goto done;

            /*
            * 1. get packet from mpipe
            * */
			unsigned char * va =  gxio_mpipe_idesc_get_va(idesc);
            uint32_t l2_length =  gxio_mpipe_idesc_get_l2_length(idesc);
            /*
            * 2. get buffer from pool
            * */
            while(unlikely(mwsr_pool_dequeue(generator->pool,(void **)&packet) !=  0))
            {
			    continue;
            }

            //printf("[rank:%d]packet:%p,packet->data:%p,va:%p,l2_length:%d\n",rank,packet,packet->data,va,l2_length);
            packet->pool   = generator->pool;
            memcpy(packet->data + PAY_LEN,va,l2_length);
            packet->length = l2_length;

            /*
            * 3. 数据放到下一步的队列里。
            * */
            /* 数据包均匀 分部到 下一个工作的线程里。*/
    		if(global_config -> pipe_depth  > 1)
    		{
            	parser_t * parser = &parser_group->parser[next_thread_id];
    			next_thread_id = (next_thread_id + g_nums < p_nums) ?
                                 (next_thread_id + g_nums) : 
                                 generator->index;
    			while(unlikely(swsr_queue_enqueue(parser->queue,packet) != 0))
    			{
    				continue;	
    			}
    		}
    		else
    		{
        		while(unlikely(mwsr_pool_enqueue(packet->pool,packet) != 0))
    			{
    				continue;	
    			}
    		}
            generator->total_send_byte += l2_length;
    		generator->alive++;

			gxio_mpipe_iqueue_drop(iqueue, idesc);
done:
            gxio_mpipe_iqueue_consume(iqueue, idesc);
        }
        else
        {
            continue;
        }
    }
    
}
#else
void intel_packet_collector(generator_t * generator)
{
    int rank = generator->rank;
    int dev_id = generator->dev_id;
    char * va = NULL;
    int32_t l2_length = 0;
    packet_t * packet;
    sim_config_t * config = generator->config;
	int g_nums = config->generator_nums;
	int p_nums = config->parser_nums;
	int next_thread_id = generator->index;
    parser_group_t * parser_group = get_parser_group();
    if(parser_group == NULL)
    {
        printf("parser_group is null,exit now\n");
        exit(0);
    }
    make_clean_packet(generator);
    while(1)
    {
        /*
        * 1. get packet from inac0
        * */
        va        = inac_get_l2_pkt(dev_id,rank);
        l2_length = inac_get_l2_size(dev_id,rank);
        if((va != NULL) && (l2_length > 40))
        {
            /*
            * 2. get buffer from pool
            * */
            while(unlikely(mwsr_pool_dequeue(generator->pool,(void **)&packet) !=  0))
            {
			    continue;
            }

            //printf("[rank:%d]packet:%p,packet->data:%p,va:%p,l2_length:%d\n",rank,packet,packet->data,va,l2_length);
            packet->pool   = generator->pool;
            memcpy(packet->data + PAY_LEN,va,l2_length);
            packet->length = l2_length;

            /*
            * 3. 数据放到下一步的队列里。
            * */
            /* 数据包均匀 分部到 下一个工作的线程里。*/
    		if(global_config -> pipe_depth  > 1)
    		{
            	parser_t * parser = &parser_group->parser[next_thread_id];
    			next_thread_id = (next_thread_id + g_nums < p_nums) ?
                                 (next_thread_id + g_nums) : 
                                 generator->index;
    			while(unlikely(swsr_queue_enqueue(parser->queue,packet) != 0))
    			{
    				continue;	
    			}
    		}
    		else
    		{
        		while(unlikely(mwsr_pool_enqueue(packet->pool,packet) != 0))
    			{
    				continue;	
    			}
    		}
            generator->total_send_byte += l2_length;
    		generator->alive++;
        }
        else
        {
            usleep(1);
        }
        generator->alive++;
    }
}
#endif
static void generator_mode(generator_t * generator,int data_len)
{
    sim_config_t * config = generator->config;
    if(config->protocol == IPPROTO_TCP)
    {
        /*
        * 因为发包程序不会一会发TCP一会发UDP，是固定的，
        * 这样会减少判断。
        * 或许，一亿次循环能减少一秒把。(@_@)
        * */
        packet_generator(generator,data_len,generator_tcp_packet);

    }
    else if(config->protocol == IPPROTO_UDP)
    {
        packet_generator(generator,data_len,generator_udp_packet);
    }
    else 
    {
        printf("NOT UDP NOT TCP config error\n");
        exit(-1);
    }
}


void * packet_generator_loop(void * arg)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    generator_t * generator = (generator_t *)arg;
    sim_config_t * config = generator->config;


    if(config->packet_generator_mode == COLLECTOR_MODE)
    {
#ifdef TILERA_PLATFORM
    	tmc_sync_barrier_wait(&gbarrier);
        tilera_packet_collector(generator);
#else
        intel_packet_collector(generator);
#endif
    }
    else if(config->packet_generator_mode == GENERATOR_MODE)
    {
#ifdef TILERA_PLATFORM
    	tmc_sync_barrier_wait(&gbarrier);
#endif
        int data_len = config->pktlen + PAY_LEN + sizeof(packet_t);
        srand((unsigned int)time(NULL));
        /* loop here */
        generator_mode(generator,data_len);
    }
    pthread_exit(NULL);
}

static inline void init_signle_generator(generator_group_t * generator_group,int i,int dev_id)
{
	generator_group->generator[i].pool = memalign(64, sizeof(mwsr_pool_t));
	assert(generator_group->generator[i].pool);
	mwsr_pool_init(generator_group->generator[i].pool);
	int item_size = global_config->pktlen + PAY_LEN + sizeof(packet_t);
	int pool_size   = 1024;
    char * buffer = malloc(pool_size * item_size);
    exit_if_ptr_is_null(buffer,"alloc pool buffer error");
    int j = 0;
    for(j = 0; j < pool_size; ++j)
    {
    /*
    * 这个复杂的复制是为了，让node_t[]数组里面的指针指向真实的buffer.
    * */
		mwsr_pool_enqueue(generator_group->generator[i].pool, buffer + j * item_size);
    }

    generator_group->generator[i].config = malloc(sizeof(sim_config_t)); 
    exit_if_ptr_is_null(generator_group->generator[i].config,"config error");
    memcpy(generator_group->generator[i].config,global_config,sizeof(sim_config_t));
    generator_group->generator[i].index = i;
    generator_group->generator[i].next_thread_id = 0;
    generator_group->generator[i].total_send_byte = 0;
    generator_group->generator[i].rank = i;
    generator_group->generator[i].dev_id = dev_id;
    generator_group->generator[i].mpipe = global_mpipe;
}

generator_group_t * init_generator_group(sim_config_t * config)
{
    int32_t dev_id = -1;
    if(generator_group != NULL)
    {
        printf("generator_group is alread");
        pthread_mutex_unlock(&global_create_generator_lock);
        return NULL;
    }
    int i = 0;
    int numbers = config->generator_nums;
    generator_group = malloc(sizeof(generator_group_t));
    /*
     * 初始化一个缓冲区池。
     * 这个缓冲区的头部是个结构体指针，下面是packet_length的长度的缓冲区。
     * */
    generator_group->generator = malloc(sizeof(generator_t) * numbers);
    exit_if_ptr_is_null(generator_group->generator,
                        "generator_group.generator error");
    generator_group->numbers   = numbers;
    generator_group->config    = config;

#ifdef TILERA_PLATFORM
    /*
    * 即使在tilera平台仍然可以采用产生包模式。
    * */
    global_mpipe = malloc(sizeof(mpipe_common_t));
    exit_if_ptr_is_null(global_mpipe,
                        "--------malloc mpipe error--------------"
                        );
    if(config->packet_generator_mode == COLLECTOR_MODE)
    {
        init_mpipe_config(global_mpipe,config);
        init_mpipe_resource(global_mpipe);    
    }
#else
    if(config->packet_generator_mode == COLLECTOR_MODE)
    {
        int32_t result = -1;
        result = inac_open("inac0", &dev_id);
        if(result != 0)
        {
            printf("[%s-%d] result: %d, dev_id : %d\n",__func__,
                                                       __LINE__,
                                                       result, 
                                                       dev_id);
            exit(-2);
        }
    }
#endif

    for(i = 0; i < numbers; ++i)
    {
		init_signle_generator(generator_group,i,dev_id);
        if(pthread_create(&generator_group->generator[i].id,
                      NULL,
                      packet_generator_loop,
                      &generator_group->generator[i]) != 0)
        {
            printf("Init Packet Generator thread failed. Exit Now.\n");
            exit(0);
        }
    }
    return generator_group;
}

generator_group_t * get_generator_group()
{
    return generator_group;
}

