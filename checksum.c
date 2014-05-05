/*************************************************************************
	> File Name: checksum.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 05 May 2014 03:22:26 PM CST
 ************************************************************************/

#include "includes.h"

void checksum_ip(struct iphdr * iph)
{
    int ihl = iph->ihl;
    uint16_t * ipheader = (uint16_t *)iph;
    int i = 0;
    uint32_t sum = 0;
    printf("iph->ihl:%d\n",iph->ihl);
    iph->check = 0;
    for(i = 0; i < iph->ihl * 2; ++i)
    {
        printf("iph[%d] = %4x\n",i,ntohs(ipheader[i]));
        sum += (ipheader[i]);
    }
    while(sum >> 16)
        sum = (sum >> 16) + (sum & 0xffff);
    iph->check = ~sum;
}

