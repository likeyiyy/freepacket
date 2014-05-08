/*************************************************************************
	> File Name: packet_generator.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:16 PM CST
 ************************************************************************/

#include "includes.h"
static uint8_t  * config_file = CONFIG_FILE;
config_t * config;
generator_info_t generator_info;
extern pool_t * packet_pool;
extern parser_set_t * parser_set;
    
void init_generator(int numbers)
{
    int i = 0;

    config = malloc(sizeof(config_t));
    exit_if_ptr_is_null(config,"config error");
    read_config_file(config_file,config);
    config->numbers = PACKET_POOL_SIZE;  
    /*
     * 初始化一个缓冲区池。
     * 这个缓冲区的头部是个结构体指针，下面是packet_length的长度的缓冲区。
     * */
    packet_pool = init_pool(PACKET_POOL,config->numbers,config->pktlen + sizeof(packet_t));
    generator_info.generator = malloc(sizeof(generator_t) * numbers);
    exit_if_ptr_is_null(generator_info.generator,"generator_info.generator error");

    generator_info.numbers   = numbers;

    for(i = 0; i < numbers; ++i)
    {
        generator_info.generator[i].pool = packet_pool;
        generator_info.generator[i].config = config;
        generator_info.generator[i].parser_set = parser_set;
        generator_info.generator[i].index = i;
        generator_info.generator[i].next_thread_id = 0;
        generator_info.generator[i].total_send_byte = 0;
        pthread_create(&generator_info.generator[i].id,
                      NULL,
                      packet_generator_loop,
                      &generator_info.generator[i]); 
    }
}
int pop_payload(void * payload,unsigned char * data,config_t * config)
{
    int i = strlen(data);
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
static inline uint32_t get_next_srcip(config_t * config)
{
    return config->saddr_cur == config->saddr_max ?
    config->saddr_cur = config->saddr_min:
    ++config->saddr_cur;
}
static inline uint32_t get_next_dstip(config_t * config)
{
    return config->daddr_cur == config->daddr_max ?
    config->daddr_cur = config->daddr_min:
    ++config->daddr_cur;
}
static inline uint16_t get_next_srcport(config_t * config)
{
    return config->sport_cur == config->sport_max ?
    config->sport_cur = config->sport_min:
    ++config->sport_cur;
}
static inline uint16_t get_next_dstport(config_t * config)
{
    return config->dport_cur == config->dport_max ?
    config->dport_cur = config->dport_min:
    ++config->sport_cur;
}
static int pop_transmission_tcp(void * tcph,config_t * config)
{
    struct tcphdr * tcp = (struct tcphdr *)tcph;
    tcp->source = htons(get_next_srcport(config));
    tcp->dest   = htons(get_next_dstport(config));
    tcp->doff  = sizeof(struct tcphdr) / 4;
    tcp->check = 0;
    return config->pktlen - 34;
}
static int pop_transmission_udp(void * udph,config_t * config)
{
    struct udphdr * udp = (struct udphdr *)udph;
    udp->source = htons(get_next_srcport(config));
    udp->dest   = htons(get_next_dstport(config));
    udp->len    = htons(config->pktlen - 34);
    udp->check  = 0;
    return config->pktlen - 34;
}
static void pop_iplayer_tcp(void * iph,config_t * config)
{
    struct iphdr * ip = (struct iphdr *)iph;
    ip->version = IPVERSION;
    ip->ihl     = sizeof(struct iphdr) / sizeof(uint32_t);
    ip->tot_len = htons(config->pktlen-14);
    ip->ttl     = IPDEFTTL;
    ip->protocol = IPPROTO_TCP;
    ip->saddr   = htonl(get_next_srcip(config));
    ip->daddr   = htonl(get_next_dstip(config));
    ip->check   = ~ip_xsum((uint16_t *)ip,sizeof(struct iphdr)/2,0);
    /*
    * Do TCP header Check Sum
    * */
    struct tcphdr * tcp = (struct tcphdr *)((unsigned char *)ip+20);
    uint16_t sum = 0x6 + config->pktlen - 34;
    tcp->check = (~ip_xsum((uint16_t *)&ip->saddr,(config->pktlen-26)/2,htons(sum)));
}
static void pop_iplayer_udp(void * iph,config_t * config)
{
    struct iphdr * ip = (struct iphdr *)iph;
    ip->version = IPVERSION;
    ip->ihl     = sizeof(struct iphdr) / sizeof(uint32_t);
    ip->tot_len = htons(config->pktlen-14);
    ip->ttl     = IPDEFTTL;
    ip->protocol = IPPROTO_TCP;
    ip->saddr   = get_next_srcip(config);
    ip->daddr   = get_next_dstip(config);
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
    memcpy(eth_hdr->h_dest,config->srcmac,ETH_ALEN);
    memcpy(eth_hdr->h_source,config->dstmac,ETH_ALEN);
    eth_hdr->h_proto = htons(ETH_P_IP);
}
void * packet_generator_loop(void * arg)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    generator_t * generator = (generator_t *)arg;
    config_t * config = generator->config;
    packet_t * packet;
    int payload_length;
    int tcp_length,udp_length;
    int data_len = config->pktlen + sizeof(packet_t);
    if(config->protocol == IPPROTO_TCP)
    {
        /*
        * 因为发包程序不会一会发TCP一会发UDP，是固定的，
        * 这样会减少判断。
        * 或许，一亿次循环能减少一秒把。(@_@)
        * */
        while(1)
        {
        /*
        * 1. get buffer from pool
        * */
            get_buf(generator->pool,(void **)&packet);
            bzero(packet,data_len);
            packet->length = config->pktlen;
            packet->data   = (unsigned char *)packet + sizeof(packet_t);
        /*
        * 2. 根据配置文件比如UDP，TCP来产生包结构。
        * */
            //payload_length = pop_payload(packet->data+54,config->pkt_data,config);

            tcp_length = pop_transmission_tcp(packet->data+34,config);

            pop_iplayer_tcp(packet->data+14,config);

            pop_datalink(packet->data,config);
        /*
        * 3. 数据放到下一步的队列里。
        * */
            /* 数据包均匀 分部到 下一个工作的线程里。*/
            //printf("---------------%d----------\n",generator->next_thread_id);
            parser_t * parser = &generator->parser_set->parser[generator->next_thread_id++];
            generator->next_thread_id = (generator->next_thread_id == generator->parser_set->numbers)? 0 : generator->next_thread_id;
            push_to_queue(parser->queue,packet);
            generator->total_send_byte += config->pktlen;

            pthread_testcancel();
        }
    }
    else if(config->protocol == IPPROTO_UDP)
    {
        while(1)
        {
        /*
        * 1. get buffer from pool
        * */
            get_buf(generator->pool,(void **)&packet);
            bzero(packet,data_len);
            packet->length = config->pktlen;
            packet->data   = (unsigned char *)packet + sizeof(packet_t);
        /*
        * 2. 根据配置文件比如UDP，TCP来产生包结构。
        * */
            payload_length = pop_payload(packet->data+42,config->pkt_data,config);

            udp_length = pop_transmission_udp(packet->data+34,config);

            pop_iplayer_udp(packet->data+14,config);
            
            pop_datalink(packet->data,config);

        /*
        * 3. 数据放到下一步的队列里。
        * */
            //printf("---------------%d----------\n",generator->next_thread_id);
            parser_t * parser = &generator->parser_set->parser[generator->next_thread_id++];
            generator->next_thread_id = (generator->next_thread_id == generator->parser_set->numbers)? 0 : generator->next_thread_id;
            push_to_queue(parser->queue,(void*)packet);
            pthread_testcancel();
        }

    }
}
