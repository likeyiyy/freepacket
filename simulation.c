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

    int manager_num,parser_num,generator_num;
    manager_num = parser_num = generator_num = 1;
    int ch;
    while((ch = getopt(argc,argv,"m:p:g:")) != -1)
    {
        switch(ch)
        {
            case 'm': manager_num = atoi(optarg);break;
            case 'p': parser_num  = atoi(optarg);break;
            case 'g': generator_num = atoi(optarg);break;
            default:printf("other option :%c\n",ch);break;
        }
    }
    init_manager_set(manager_num);

    init_parser_set(parser_num);

    /* Generator */
    init_generator_set(generator_num);

    sys_dispaly(generator_set,parser_set,manager_set);
    while(1)
    {
        sleep(10);
    }
    pthread_exit(NULL);
}
