/*************************************************************************
	> File Name: checksum_test.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 05 May 2014 03:25:19 PM CST
 ************************************************************************/

#include "includes.h"
uint8_t ip_header[20] = 
{
    0x45,
    0x00,

    0x00,
    0x4c,

    0xac,
    0xfc,

    0x40,
    0x00,

    0x2e,
    0x06,

    0x00,
    0x00,

    0x73,
    0xef,
    0xd2,
    0xdb,

    0xc0, 
    0xa8,
    0x67,
    0x9e
};
unsigned char buffer[2048];
#if 0
void print_packet(unsigned char * packet,int size) 
{        
    int i = 0;  
    for(i = 0; i < size; i++)                       
    {   
                    if((i == 0)||(i % 16))                      
                    {           
                                                printf("%02x ",packet[i]);              
                                            }           
                    else        
                    {           
                                                printf("\n%02x ",packet[i]);            
                                            }           
                    
        }   
    printf("\n");                                   
} 
#endif
int main()
{

    printf("sizeof short:%d\n",sizeof(short));
    
    struct iphdr * iph = (struct iphdr *)ip_header;

    int sockfd = socket(PF_PACKET,SOCK_DGRAM,htons(ETH_P_IP));
    struct sockaddr_ll sa;

    bzero(&sa,sizeof(struct sockaddr_ll));

    sa.sll_family = PF_PACKET;
    sa.sll_protocol = htons(ETH_P_IP);

    
    int len = sizeof(struct sockaddr_ll);
    int total;
    while(1)
    {
    
    
        total = recvfrom(sockfd,buffer,2048,0,(struct sockaddr*)&sa,&len);

        struct iphdr * ip = (struct iphdr *)buffer;       
        printf("ip->tot_len:%d\n",ip->tot_len);
        if((ip->protocol == IPPROTO_TCP) )
        {
            print_packet(buffer,total); 
            struct tcphdr * tcp_hdr = (struct tcphdr *)((unsigned char *)ip + 20);
            parse_iphdr(ip);
            parse_tcphdr(tcp_hdr);
            printf("checksum:%x\n",((struct iphdr *)buffer)->check);
            checksum_ip((struct iphdr *)buffer);
            print_packet(buffer,total);
            tcp_hdr->check = 0;

            uint16_t sum = IPPROTO_TCP + ntohs(ip->tot_len) - 20;
            uint16_t l4_check = ~ip_xsum((uint16_t *)&ip->saddr,(total - 12)/2,htons(sum));

            tcp_hdr->check = l4_check;

            printf("l4 check:%x\n",l4_check);

            printf("checksum:%x\n",((struct iphdr *)buffer)->check);


            print_packet(buffer,total);
            break;
        }
    }
    return 0;
}
