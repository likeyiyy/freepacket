/*************************************************************************
	> File Name: packet_generator.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:16 PM CST
 ************************************************************************/
#include "includes.h"
//#define memcpy(a,b,c) do { memcpy(a,b,c);printf("packet_generator_loop memcpy here\n"); } while(0)
generator_set_t * generator_set;
extern pool_t * packet_pool;
extern parser_set_t * parser_set;
void   destroy_generator(generator_set_t * generator_set)
{
    int i = 0;
    for(i = 0; i < generator_set->numbers; ++i)
    {
        generator_set->generator[i].config     = NULL;
        generator_set->generator[i].pool       = NULL;
        generator_set->generator[i].parser_set = NULL;
    }
    free(generator_set->generator);
    generator_set->generator = NULL;
    /* 销毁缓冲区池子 */
    /*
     * 销毁配置文件分配的内存。
     * */
    free(generator_set->config->pkt_data);
    generator_set->config->pkt_data = NULL;
    free(generator_set->config);
    generator_set->config = NULL;
    free(generator_set);
    generator_set = NULL;
}
void   finish_generator(generator_set_t * generator_set)
{
    int i = 0;
    for(i = 0; i < generator_set->numbers; ++i)
    {
        pthread_cancel(generator_set->generator[i].id);
    }
}
int pop_payload(void * payload, char * data,config_t * config)
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
static inline uint32_t get_next_srcip(config_t * config)
{
    return config->saddr_cur == config->saddr_max ?
    config->saddr_cur = config->saddr_min:
    ++config->saddr_cur;
}
#define GET_NEXT_DSTIP(config) \
        ((config->daddr_cur == config->daddr_max) ? \
         (config->daddr_cur = config->daddr_min) : \
         (++config->daddr_cur))
static inline uint32_t get_next_dstip(config_t * config)
{
    return config->daddr_cur == config->daddr_max ?
    config->daddr_cur = config->daddr_min:
    ++config->daddr_cur;
}
#define GET_NEXT_SRCPORT(config) \
        ((config->sport_cur == config->sport_max) ? \
         (config->sport_cur = config->sport_min) : \
         (++config->sport_cur))
static inline uint16_t get_next_srcport(config_t * config)
{
    return config->sport_cur == config->sport_max ?
    config->sport_cur = config->sport_min:
    ++config->sport_cur;
}
#define GET_NEXT_DSTPORT(config) \
        ((config->dport_cur == config->dport_max) ? \
         (config->dport_cur = config->dport_min) : \
         (++config->dport_cur))
static inline uint16_t get_next_dstport(config_t * config)
{
    return config->dport_cur == config->dport_max ?
    config->dport_cur = config->dport_min:
    ++config->dport_cur;
}
static inline int pop_transmission_tcp(void * tcph,config_t * config)
{
    struct tcphdr * tcp = (struct tcphdr *)tcph;
    tcp->source = htons(GET_NEXT_SRCPORT(config));
    tcp->dest   = htons(GET_NEXT_DSTPORT(config));
    tcp->doff  = sizeof(struct tcphdr) / 4;
    tcp->check = 0;
    return config->pktlen - 34;
}
static inline int pop_transmission_udp(void * udph,config_t * config)
{
    struct udphdr * udp = (struct udphdr *)udph;
    udp->source = htons(GET_NEXT_SRCPORT(config));
    udp->dest   = htons(GET_NEXT_DSTPORT(config));
    udp->len    = htons(config->pktlen - 34);
    udp->check  = 0;
    return config->pktlen - 34;
}
static inline void pop_iplayer_tcp(void * iph,config_t * config)
{
    struct iphdr * ip = (struct iphdr *)iph;
    ip->version = IPVERSION;
    ip->ihl     = sizeof(struct iphdr) / sizeof(uint32_t);
    ip->tot_len = htons(config->pktlen-14);
    ip->ttl     = IPDEFTTL;
    ip->protocol = IPPROTO_TCP;
    ip->saddr   = htonl(GET_NEXT_SRCIP(config));
    ip->daddr   = htonl(GET_NEXT_DSTIP(config));
    ip->check   = ~ip_xsum((uint16_t *)ip,sizeof(struct iphdr)/2,0);
    /*
    * Do TCP header Check Sum
    * */
    struct tcphdr * tcp = (struct tcphdr *)((unsigned char *)ip+20);
    uint16_t sum = 0x6 + config->pktlen - 34;
    tcp->check = (~ip_xsum((uint16_t *)&ip->saddr,(config->pktlen-26)/2,htons(sum)));
}
static inline void pop_iplayer_udp(void * iph,config_t * config)
{
    struct iphdr * ip = (struct iphdr *)iph;
    ip->version = IPVERSION;
    ip->ihl     = sizeof(struct iphdr) / sizeof(uint32_t);
    ip->tot_len = htons(config->pktlen-14);
    ip->ttl     = IPDEFTTL;
    ip->protocol = IPPROTO_UDP;
    ip->saddr   = htonl(GET_NEXT_SRCIP(config));
    ip->daddr   = htonl(GET_NEXT_DSTIP(config));
    ip->check   = ~ip_xsum((uint16_t *)ip,sizeof(struct iphdr)/2,0);
    /*
    * Do UDP header Check Sum
    * */
    struct udphdr * udp = (struct udphdr *)((unsigned char *)ip+20);
    uint16_t sum = 0x17 + config->pktlen - 34;
    udp->check = (~ip_xsum((uint16_t *)((unsigned char *)ip+12),(config->pktlen-26)/2,htons(sum)));
}
static inline void pop_datalink(void * packet,config_t * config)
{
    struct ethhdr * eth_hdr = (struct ethhdr *)(packet);
    memcpy(eth_hdr->h_dest,config->dstmac,ETH_ALEN);
    memcpy(eth_hdr->h_source,config->srcmac,ETH_ALEN);
    eth_hdr->h_proto = htons(ETH_P_IP);
}
typedef void (GenerHandler) (packet_t * packet,config_t * config);
static inline void generator_tcp_packet(packet_t * packet,config_t * config)
{
    //pop_payload(packet->data+54,config->pkt_data,config);
    pop_transmission_tcp(packet->data + 34,config);
    pop_iplayer_tcp(packet->data + 14,config);
    pop_datalink(packet->data,config);
}

static inline void generator_udp_packet(packet_t * packet,config_t * config)
{
    //pop_payload(packet->data+42,config->pkt_data,config);
    pop_transmission_udp(packet->data+34,config);
    pop_iplayer_udp(packet->data+14,config);
    pop_datalink(packet->data,config);
}

static void packet_generator(generator_t * generator,int data_len,GenerHandler * Handler)
{
    packet_t * packet;
    //struct timespec oldtime,newtime;
    uint64_t old,new;
    config_t * config = generator->config;
    //old = GET_CYCLE_COUNT(); 
    while(1)
    {
       old = GET_CYCLE_COUNT();
       /*
       * 1. get buffer from pool
       * */
       if(get_buf(generator->pool,NO_WAIT_MODE,(void **)&packet) < 0)
       {
           generator->drop_total++;
           continue;
       }
       //bzero(packet,data_len);
       packet->pool   = generator->pool;
       packet->length = config->pktlen;
       packet->data   = (unsigned char *)packet + sizeof(packet_t);
        /*
        * 2. 根据配置文件比如UDP，TCP来产生包结构。
        * */
        Handler(packet,config);
        /*
        * 3. 数据放到下一步的队列里。
        * */
        /* 数据包均匀 分部到 下一个工作的线程里。*/
        parser_t * parser = &generator->parser_set->parser[generator->next_thread_id++];
        generator->next_thread_id = (generator->next_thread_id == generator->parser_set->numbers)? 0 : generator->next_thread_id;
        push_to_queue(parser->queue,packet);
        generator->total_send_byte += config->pktlen;
        /*4. 延时统计函数 */
        new = GET_CYCLE_COUNT() - old;
        while((int64_t)new - (int64_t)generator->config->period < 0)
        { 
            new = GET_CYCLE_COUNT() - old;
        } 
    }
}

#ifdef TILERA_PLATFORM
static void tilera_packet_collector(generator_t * generator)
{
    packet_t * packet;
    mpipe_common_t * mpipe = generator->mpipe;

    gxio_mpipe_iqueue_t * iqueue = mpipe->iqueues[generator->rank];
    config_t * config = generator->config;
    gxio_mpipe_idesc_t * idesc;

    while(1)
    {
		if(gxio_mpipe_iqueue_try_peek(iqueue,&idesc) > 0)
        {
            if (gxio_mpipe_iqueue_drop_if_bad(iqueue, idesc))
                goto done;
            if(get_buf(generator->pool,NO_WAIT_MODE,(void **)&packet) < 0)
            {
                generator -> drop_total++;
			    gxio_mpipe_iqueue_drop(iqueue, idesc);
                goto done;
            }

			unsigned char * va =  gxio_mpipe_idesc_get_va(idesc);
            uint32_t l2_length =  gxio_mpipe_idesc_get_l2_length(idesc);

            packet->pool   = generator->pool;
            memcpy(packet->data,va,l2_length);
            packet->length = l2_length;

            /* 数据包均匀 分部到 下一个工作的线程里。*/
            parser_t * parser = &generator->parser_set->parser[generator->next_thread_id++];
            generator->next_thread_id = (generator->next_thread_id == generator->parser_set->numbers)? 0 : generator->next_thread_id;
            push_to_queue(parser->queue,packet);
            generator->total_send_byte += config->pktlen;
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
#endif
static void generator_mode(generator_t * generator,int data_len)
{
    config_t * config = generator->config;
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
    mpipe_common_t * mpipe = generator->mpipe;
    config_t * config = generator->config;


    if(config->packet_generator_mode == COLLECTOR_MODE)
    {
#ifdef TILERA_PLATFORM
    /**
     * Bind to a single cpu
     * */
        int rank = generator->rank;
        int cpu = tmc_cpus_find_nth_cpu(&mpipe->cpus, rank);
        int result = tmc_cpus_set_my_cpu(cpu);
        VERIFY(result, "tmc_cpus_set_my_cpu()");
        tmc_sync_barrier_wait(&mpipe->barrier);
        tilera_packet_collector(generator);

#else
        printf("NOW just support tilera platform collector mode\n");
        exit(0);
#endif
    }
    else if(config->packet_generator_mode == GENERATOR_MODE)
    {
        int data_len = config->pktlen + sizeof(packet_t);
        srand((unsigned int)time(NULL));
        /* loop here */
        generator_mode(generator,data_len);
    }
    pthread_exit(NULL);
}

void init_generator_set(config_t * config)
{
    int i = 0;
    int numbers = config->generator_workers;
    generator_set = malloc(sizeof(generator_set_t));
    /*
     * 初始化一个缓冲区池。
     * 这个缓冲区的头部是个结构体指针，下面是packet_length的长度的缓冲区。
     * */
    generator_set->generator = malloc(sizeof(generator_t) * numbers);
    exit_if_ptr_is_null(generator_set->generator,"generator_set.generator error");
    generator_set->numbers   = numbers;
    generator_set->config    = config;

#ifdef TILERA_PLATFORM
    /*
    * 即使在tilera平台仍然可以采用产生包模式。
    * */
    mpipe_common_t * mpipe = NULL;
    mpipe = malloc(sizeof(mpipe_common_t));
    exit_if_ptr_is_null(mpipe,"--------malloc mpipe error--------------");
    if(config->packet_generator_mode == COLLECTOR_MODE)
    {
        init_mpipe_config(mpipe,config);
        init_mpipe_resource(mpipe);    
        tmc_sync_barrier_init(&mpipe->barrier,mpipe->num_workers);
    }
#endif

    for(i = 0; i < numbers; ++i)
    {
        generator_set->generator[i].pool = init_pool(PACKET_POOL,
                                                config->packet_pool_size,
                                                config->pktlen + sizeof(packet_t));
        generator_set->generator[i].pool->pool_type = PACKET_POOL;
        generator_set->generator[i].config = malloc(sizeof(config_t)); 
        exit_if_ptr_is_null(generator_set->generator[i].config,"config error");
        memcpy(generator_set->generator[i].config,config,sizeof(config_t));
        generator_set->generator[i].parser_set = parser_set;
        generator_set->generator[i].index = i;
        generator_set->generator[i].next_thread_id = 0;
        generator_set->generator[i].total_send_byte = 0;
#ifdef TILERA_PLATFORM
        generator_set->generator[i].rank = i;
        generator_set->generator[i].mpipe = mpipe;
#endif
        if(pthread_create(&generator_set->generator[i].id,
                      NULL,
                      packet_generator_loop,
                      &generator_set->generator[i]) != 0)
        {
            printf("Init Packet Generator thread failed. Exit Now.\n");
            exit(0);
        }
    }
}
