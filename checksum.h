/*************************************************************************
	> File Name: checksum.h
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 05 May 2014 03:22:30 PM CST
 ************************************************************************/

#ifndef CHECK_SUM_H
#define CHECK_SUM_H

void checksum_ip(struct iphdr * iph);
void checksum_tcp(struct tcphdr * tcph,int length);
uint16_t ip_xsum (uint16_t *buff, int len, uint32_t xsum);
uint32_t crc32_32(uint32_t * buf, uint32_t len);
uint32_t GetCRC32(uint8_t * buf, uint32_t len);

#endif
