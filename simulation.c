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
int top_argc = 0;
char ** top_argv;


int main(int argc, char ** argv)
{
    init_manager_set(17);
    init_parser_set(10);
    init_generator_set(10);
    sys_dispaly(generator_set,parser_set,manager_set);
    while(1)
    {
        sleep(10);
    }
    pthread_exit(NULL);
}
