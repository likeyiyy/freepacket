#include "includes.h"

sim_config_t * global_config;
void exit_if_ptr_is_null(void * ptr,const char * message) 
{
    if(ptr == NULL)
    {   
        printf("%s\n",message);
        exit(-1);
    }   
}
static inline char * skip_var_name(const char * p)
{
    while(!isspace(p[0]) && p[0] != '=' && p[0] != '\0')
    {
        p++;
    }
    return (char *)p;
}
static inline char * skip_opeartor(const char * p)
{
    while((isspace(p[0])||p[0] == '=') && p[0] != '\0')
    {
        p++;
    } 
    return (char *)p;
}
static inline char * strupr(char * p)
{
    char * q = p;
    while(*p)
    {
        *p = toupper(*p);
        p++;
    }
    return q;
}
/*
 *  * Convert Ethernet address string representation to binary data
 *   * @param    a    string in xx:xx:xx:xx:xx:xx notation
 *    * @param    e    binary data
 *     * @return    TRUE if conversion was successful and FALSE otherwise
 *      */
int ether_atoe(const char *a, unsigned char *e)
{
      char *c = (char *) a;
        int i = 0;
          memset(e, 0, ETH_ALEN);
          for (;;) {
                  e[i++] = (unsigned char) strtoul(c, &c, 16);
                      if (!*c++ || i == ETH_ALEN)
                                break;
          }
            return (i == ETH_ALEN);
}
/*
 *  * Convert Ethernet address binary data to string representation
 *   * @param    e    binary data
 *    * @param    a    string in xx:xx:xx:xx:xx:xx notation
 *     * @return    a
 *      */
char * ether_etoa(const unsigned char *e, char *a)
{
      char *c = a;
        int i;
        for (i = 0; i < ETH_ALEN; i++) {
                if (i)
                          *c++ = ':';
                    c += sprintf(c, "%02X", e[i] & 0xff);
        }
          return a;
}
void print_config_file(sim_config_t * config)
{
    char macaddr[18];
    struct in_addr min,max;
    if(config->protocol == IPPROTO_TCP)
    {
        printf("protocol:tcp\n");
    }
    else if(config->protocol == IPPROTO_UDP)
    {
        printf("protocol:udp\n");
    }
    printf("srcmac:%s\n",ether_etoa(config->srcmac,macaddr));
    printf("dstmac:%s\n",ether_etoa(config->dstmac,macaddr));
    min.s_addr = ntohl(config->saddr_min);
    max.s_addr = ntohl(config->saddr_max);
    printf("srcipaddr.min:%s\t",inet_ntoa(min));
    printf("srcipaddr.max:%s\n",inet_ntoa(max));
    min.s_addr = ntohl(config->daddr_min);
    max.s_addr = ntohl(config->daddr_max);

    printf("dstipaddr.min:%s\t",inet_ntoa(min));
    printf("dstipaddr.max:%s\n",inet_ntoa(max));
    printf("sport_min:%d,sport_max:%d\n",config->sport_min,config->sport_max);
    printf("dport_min:%d,dport_max:%d\n",config->dport_min,config->dport_max);

    printf("pktlen:%d\n",config->pktlen);
    printf("speed:%d\n",config->speed);
    printf("pkt_data:%s\n",config->pkt_data);

	printf("period:%d\n",config->period);
	printf("num_workers:%d\n",config->generator_nums);
#ifdef TILERA_PLATFORM
	printf("notif_ring_entries:%d\n",config->notif_ring_entries);
	printf("equeue_entries:%u\n",config->equeue_entries);
	printf("per_worker_buckets:%d\n",config->per_worker_buckets);
	printf("once packet nums:%d\n",config->once_packet_nums);
#endif

    
}
int read_config_file(const char * file_name,sim_config_t * config)
{
    assert(config != NULL);
    assert(file_name != NULL);
    FILE * fp = NULL;
    char buf[BUFSIZ] = {0};
    char * p, *q;
    char * pname = NULL;
    int count = 0;
    if((fp = fopen(file_name,"r")) == NULL)
    {

		printf("----------file_name:%s----------\n",file_name);
        perror("can't open config file");
        exit(-1);
    }
    while(1)
    {
        fgets(buf,BUFSIZ,fp);
        if(feof(fp))
        {
            break;
        }
        count = strlen(buf);
        p = buf;
        //printf("count=%d\t%s",count,buf);
        //skip whitespace
        while(count > 0 &&isspace(p[0]))
        {
            count--;
            p++;
        }
        //printf("count=%d\t%s",count,buf+i);
        if(p[0] == '#')
        {
            continue;
        }
        q = skip_var_name(p);
        //printf("++%s++++++++++++++%s++\n",p,q);
        pname = malloc(q-p+1);
        exit_if_ptr_is_null(pname,"pname alloca error");
        strncpy(pname, p, q-p );
	    pname[q-p] = '\0';
	//printf("___%d___%s____\n",q-p,pname);
        p = q;
        p = skip_opeartor(p);
        //printf("%s\n",p);
        pname = strupr(pname);
        unsigned int ipaddr[4] = {0};
        unsigned int counter;
        uint32_t port;

	//printf("--%s--%s--\n",q,pname);
        //switch 判断var_name属于哪一类.
        if(strcmp(pname,"PROTO") == 0)
        {
            if(strstr(p,"tcp") != NULL)
            {
                config->protocol = IPPROTO_TCP;
            }
            else if(strstr(p,"udp") != NULL)
            {
                config->protocol = IPPROTO_UDP;
            }
        }
        else if(strcmp(pname,"SRCMAC") == 0)
        {
            ether_atoe(p,config->srcmac);
        }
        else if(strcmp(pname,"DSTMAC") == 0)
        {
            ether_atoe(p,config->dstmac);
        }
        else if(strcmp(pname,"SRCIP") == 0)
        {
            int rev = sscanf(p,"%u.%u.%u.%u+%u",&ipaddr[0],&ipaddr[1],
                             &ipaddr[2],&ipaddr[3],&counter); 
            if(rev == 1)
            {
                counter = 0;
            }
            else if ( rev == -1)
            {
                printf("SRCIP Read config file Error\n");
                return 0;
            }
            config->saddr_min = ipaddr[0] << 24 | ipaddr[1] << 16 | ipaddr[2] << 8 | ipaddr[3];
            config->saddr_cur = config->saddr_min;
            config->saddr_max = (config->saddr_min)+ counter;
        }
        else if(strcmp(pname,"DSTIP") == 0)
        {
            int rev = sscanf(p,"%u.%u.%u.%u+%u",&ipaddr[0],&ipaddr[1],
                             &ipaddr[2],&ipaddr[3],&counter); 
            if(rev == 1)
            {
                counter = 0;
            }
            else if ( rev == -1)
            {
                printf("SRCIP Read config file Error\n");
                return 0;
            }
            config->daddr_min = ipaddr[0] << 24 | ipaddr[1] << 16 | ipaddr[2] << 8 | ipaddr[3];
            config->daddr_cur = config->daddr_min;
            config->daddr_max = ((config->daddr_min)+ counter);
        }
        else if(strcmp(pname,"SRCPORT") == 0)
        {
            int rev = sscanf(p,"%u+%u",&port,&counter); 
            if(rev == 1)
            {
                counter = 0;
            }
            else if ( rev == -1)
            {
                printf("Read config file Error\n");
                return 0;
            }
            config->sport_min = port;
            config->sport_cur = config->sport_min;
            config->sport_max = port+ counter;
        }
        else if(strcmp(pname,"DSTPORT") == 0)
        {
            int rev = sscanf(p,"%u+%u",&port,&counter); 
            if(rev == 1)
            {
                counter = 0;
            }
            else if ( rev == -1)
            {
                printf("Read config file Error\n");
                return 0;
            }
            config->dport_min = port;
            config->dport_cur = config->dport_min;
            config->dport_max = port+ counter;
        }
        else if(strcmp(pname,"PKTLEN") == 0)
        {
            config->pktlen = atoi(p);
            if(config->pktlen > 1514)
            {
                printf("Read config file Error:pktlen too long\n");
                exit(0);
            }
        }
        else if(strcmp(pname,"PKTDATA") == 0)
        {
            if(strcmp(p,"none\n") == 0)
            {
                config->pkt_data = NULL;
            }
            config->pkt_data = calloc(strlen(p)+1,sizeof(char));
            exit_if_ptr_is_null(config->pkt_data,"config->pkt_data malloc error");
            strcpy(config->pkt_data, p);
        }
        else if(strcmp(pname,"SPEED") == 0)
        {
            config->speed = atoi(p);
        }
        else if(strcmp(pname,"LINK_NAME") == 0)
        {
            /**         NOTICE "\n"      **/
            int length = strlen(p);
            config -> link_name = calloc(length, sizeof(char));
            exit_if_ptr_is_null(config->link_name,"config->link_name malloc error");
            strncpy(config->link_name,p,length);
            config -> link_name[length - 1] = '\0';
        }
        else if(strcmp(pname,"GENERATOR_NUMS") == 0)
        {
            config->generator_nums = atoi(p);
        }
        else if(strcmp(pname,"PARSER_NUMS") == 0)
        {
            config->parser_nums = atoi(p);
        }
        else if(strcmp(pname,"MANAGER_NUMS") == 0)
        {
            config->manager_nums = atoi(p);
        }
        else if(strcmp(pname,"GENERATOR_POOL_SIZE") == 0)
        {
            config->generator_pool_size = atoi(p);
        }
        else if(strcmp(pname,"PARSER_QUEUE_LENGTH") == 0)
        {
            config->parser_queue_length = atoi(p);
        }
        else if(strcmp(pname,"PARSER_POOL_SIZE") == 0)
        {
            config->parser_pool_size = atoi(p);
        }
        else if(strcmp(pname,"MANAGER_POOL_SIZE") == 0)
        {
            config->manager_pool_size = atoi(p);
        }
        else if(strcmp(pname,"MANAGER_QUEUE_LENGTH") == 0)
        {
            config->manager_queue_length = atoi(p);
        }
        else if(strcmp(pname,"MANAGER_HASH_LENGTH") == 0)
        {
            config->manager_hash_length = atoi(p);
        }
        else if(strcmp(pname,"MANAGER_BUFFER_SIZE") == 0)
        {
            config->manager_buffer_size = atoi(p);
        }
        else if(strcmp(pname,"PACKET_GENERATOR_MODE") == 0)
        {
            config->packet_generator_mode = atoi(p);
        }

#ifdef TILERA_PLATFORM
        else if(strcmp(pname,"NOTIF_RING_ENTRIES") == 0)
        {
            config->notif_ring_entries = atoi(p);
        }
        else if(strcmp(pname,"PER_WORKER_BUCKETS") == 0)
        {
            config->per_worker_buckets = atoi(p);
        }
        else if(strcmp(pname,"EQUEUE_ENTRIES") == 0)
        {
            config->equeue_entries = (uint32_t)atoi(p);
        }
        else if(strcmp(pname,"ONCE_PACKET_NUMS") == 0)
        {
            config->once_packet_nums = (uint32_t)atoi(p);
        }
#endif 
        free(pname);        
    }
    return 0;
}

static uint32_t get_cpu_mhz()
{
    int mhz;
    int i = 0;
    uint64_t new,old;
    int delayms = 500;                            
    int total = 0;
    int loop_num = 4;
    for(i = 0; i < loop_num; i++)                      
    {                                             
        old = GET_CYCLE_COUNT();                  
        usleep(delayms * 1000);                   
        new = GET_CYCLE_COUNT();                  
        total += (new - old) / (delayms * 1000);
    }
    mhz = total/loop_num;
    return mhz;
}
#define NS 1000000000
uint32_t calc_period(double length,double rate,uint32_t thread_num)
{
     uint32_t mhz = get_cpu_mhz();
     printf("cpu mhz %d\n",mhz);
     double l = 8.0 * NS * length;
     double x = rate * 1024 * 1024.0;
     return  (uint32_t)((mhz / 1000.0) * (thread_num * l / x));
}

int init_config_s(sim_config_t * config)
{
    config->protocol = IPPROTO_TCP;
    config -> srcmac[0] = 0x10;
    config -> srcmac[1] = 0xBF;
    config -> srcmac[2] = 0x48;
    config -> srcmac[3] = 0xD7;
    config -> srcmac[4] = 0x9C;
    config -> srcmac[5] = 0xF8;

    config -> dstmac[0] = 0x10;
    config -> dstmac[1] = 0xBF;
    config -> dstmac[2] = 0x48;
    config -> dstmac[3] = 0xD7;
    config -> dstmac[4] = 0x98;
    config -> dstmac[5] = 0x3C;

    config -> saddr_min = inet_addr("192.168.103.19");
    config -> daddr_min = inet_addr("192.168.103.18");
    config -> saddr_cur = inet_addr("192.168.103.19");
    config -> daddr_cur = inet_addr("192.168.103.18");
    config -> saddr_max = inet_addr("192.168.103.19");
    config -> daddr_max = inet_addr("192.168.103.18");

    config -> sport_min = 100;
    config -> dport_min = 200;
    config -> sport_cur = 100;
    config -> dport_cur = 200;
    config -> sport_max = 100;
    config -> dport_max = 200;
    config -> speed     = 5000;
    config -> pkt_data  = NULL;

    config -> generator_pool_size = GENERATOR_POOL_SIZE;   

    config -> parser_queue_length = PARSER_QUEUE_LENGTH;
    config -> parser_pool_size    = PARSER_POOL_SIZE;

    config -> manager_queue_length = MANAGER_QUEUE_LENGTH;
    config -> manager_pool_size    = MANAGER_POOL_SIZE;
    config -> manager_hash_length  = MANAGER_HASH_LENGTH;
    config -> manager_buffer_size  = MANAGER_BUFFER_SIZE;

    config -> pktlen    = 600;
    config -> period    = 200;
    config -> link_name = "xgbe1";
    config -> packet_generator_mode = GENERATOR_MODE;
#ifdef TILERA_PLATFORM
    config -> notif_ring_entries = 512;
    config -> equeue_entries     = 2048;
    config -> per_worker_buckets = 128;
	config -> once_packet_nums   = 128;
#endif
    config -> generator_nums = 1;
    config -> parser_nums    = 1;
    config -> manager_nums   = 1;

    return 0;
    
}
