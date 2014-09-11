/*************************************************************************
	> File Name: display.h
	> Author: likeyi
	> Mail: likeyi@sina.com 
	> Created Time: Tue 10 Jun 2014 10:00:01 PM CST
 ************************************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

/*
* Fuction: Display the queue length pool length and so on
* Author:  likeyi
* Date:    2014年6月10日13:55:59
* */
void sys_display(generator_group_t * generator_group,
                parser_group_t * parser_group,
                manager_group_t * manager_group);
void flow_display(flow_item_t * flow);
#endif
