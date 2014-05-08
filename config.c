#include "includes.h"
void exit_if_ptr_is_null(void * ptr,const char * message) 
{
    if(ptr == NULL)
    {   
        DEBUG("%s\n",message);
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
void print_config_file(config_t * config)
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


}
int read_config_file(const char * file_name,config_t * config)
{
    FILE * fp = NULL;
    char buf[BUFSIZ] = {0};
    char * p, *q;
    char * pname = NULL;
    int count = 0;
    int flag = 0; 
    if((fp = fopen(file_name,"r")) == NULL)
    {
        perror("can't open config file");
        return -1;
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
        //printf("count=%d\t%s",count,p);
        q = skip_var_name(p);
        pname = malloc(q-p+1);
        exit_if_ptr_is_null(pname,"pname alloca error");
        strncpy(pname, p, q-p );
        p = q;
        p = skip_opeartor(p);
        //printf("%s\n",p);
        pname = strupr(pname);
        unsigned char ipaddr[4] = {0};
        unsigned int counter;
        uint16_t port;
        struct in_addr in;
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
            int rev = sscanf(p,"%d+%d",&port,&counter); 
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
            int rev = sscanf(p,"%d+%d",&port,&counter); 
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
            config->pkt_data = malloc(strlen(p)+1);
            exit_if_ptr_is_null(config->pkt_data,"config->pkt_data malloc error");
            strcpy(config->pkt_data,p);
        }
        else if(strcmp(pname,"SPEED") == 0)
        {
            config->speed = atoi(p);
        }
        
        free(pname);        
    }
    return 0;
}
