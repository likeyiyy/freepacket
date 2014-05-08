/*************************************************************************
	> File Name: undef_var.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Thu 08 May 2014 11:59:26 AM CST
 ************************************************************************/

#include <stdio.h>
#include <linux/ip.h>
#undef IPVERSION
#define IPVERSION 10
int main()
{

    printf("%d\n",IPVERSION);

}
