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
static   const char * config_file = CONFIG_FILE;

char * const short_options = "g:p:m:";
struct option long_options[] =
{
    {"generator_nums",      1,  NULL,  'g'},
    {"parser_nums",         1,  NULL,  'p'},
    {"manager_nums",        1,  NULL,  'm'},
    {"generator_pool_size", 1,  NULL,  'o'},
    {"parser_queue_length", 1,  NULL,  'q'},
    {"parser_pool_size",    1,  NULL,  'o'},
    {"manager_queue_length",1,  NULL,  'q'},
    {"manager_pool_size",   1,  NULL,  'o'},
    {"manager_hash_length", 1,  NULL,  'h'},
    {"manager_buffer_size", 1,  NULL,  'b'}
};
void parse_args(int argc, char * const argv[],sim_config_t * config)
{

    int c;
    while(1)
    {
        //int this_option_optind = optind ? optind : 1;
        int option_index = 0;

        c = getopt_long(argc, argv, 
                             "g:p:m:o:q:h:b:", 
                             long_options, 
                             &option_index);
        if (c == -1)
            break;

        switch (c) 
        {
            case 0:
            printf("option %s", long_options[option_index].name);
            if (optarg)
            {
                printf(" with arg %s", optarg);
            }
            printf("\n");
            break;

            case 'g':
                config->generator_nums = atoi(optarg);
                break;
            case 'p':
                config->parser_nums = atoi(optarg);
                break;
            case 'm':
                config->manager_nums = atoi(optarg);
                break;
            case 'o':
            if(strcmp(long_options[option_index].name,"generator_pool_size") == 0)
            {
                config->generator_pool_size = atoi(optarg);
            }
            else if(strcmp(long_options[option_index].name,"parser_pool_size") == 0)
            {
                config->parser_pool_size = atoi(optarg);
            }
            else if(strcmp(long_options[option_index].name,"manager_pool_size") == 0)
            {
                config->manager_pool_size = atoi(optarg);
            }
            else
            {
                printf("option %s", long_options[option_index].name);
                if (optarg)
                {
                    printf(" with arg %s", optarg);
                }
                printf("\n");
            }
                break;
            case 'q':
            if(strcmp(long_options[option_index].name,"parser_queue_length") == 0)
            {
                config->parser_queue_length = atoi(optarg);
            }
            else if(strcmp(long_options[option_index].name,"manager_queue_length") == 0)
            {
                config->manager_queue_length = atoi(optarg);
            }
            else
            {
                printf("option %s", long_options[option_index].name);
                if (optarg)
                {
                    printf(" with arg %s", optarg);
                }
                printf("\n");
            }
                
                break;
            case 'h':
                break;
                config->manager_hash_length = atoi(optarg);
            case 'b':
                break;
                config->manager_buffer_size = atoi(optarg);

            case '?':
                break; 
 
            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc) 
    {
               printf("non-option ARGV-elements: ");
               while (optind < argc)
                   printf("%s ", argv[optind++]);
               printf("\n");
           
    }

}


int main(int argc, char ** argv)
{

    /*
    * 命令行的优先级大于配置文件的优先级，
    * 所以每次先读取配置文件里的内容。
    * 然后从命令行读取，覆盖配置文件里相应的项。
    * 1. 默认值
    * 2. 配置文件值
    * 3. 命令行参数值
    * */
    global_config = malloc(sizeof(sim_config_t));
    exit_if_ptr_is_null(global_config,"config error");
    bzero(global_config,sizeof(sim_config_t));

    init_config_s(global_config);

    read_config_file(config_file,global_config);

    parse_args(argc,argv,global_config);

    global_config->period  = calc_period(global_config->pktlen,
                                         global_config->speed,
                                         global_config->generator_nums);

    init_manager_set(global_config);

    init_parser_set(global_config);

    /* Generator */
    init_generator_set(global_config);

#ifdef INTEL_PLATFORM
    sys_dispaly(generator_set,parser_set,manager_set);
#endif

    while(1)
    {
        sleep(10);
    }
    pthread_exit(NULL);
}
