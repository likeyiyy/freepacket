/*************************************************************************
	> File Name: sim_top.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Tue 13 May 2014 04:07:01 PM CST
 ************************************************************************/

#include "includes.h"
extern manager_set_t * manager_set;
extern generator_set_t * generator_set;
extern parser_set_t * parser_set;
extern config_t * config;
int main(int argc, char ** argv)
{
    init_manager_set(1);
    init_parser_set(1);
    init_generator_set(1);
    pthread_exit(NULL);
}
