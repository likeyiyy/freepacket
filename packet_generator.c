/*************************************************************************
	> File Name: packet_generator.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Wed 30 Apr 2014 03:02:16 PM CST
 ************************************************************************/

#include "includes.h"
static uint8_t  * config_file;
static config_t * config;
static generator_info_t generator_info;
extern pool_t * packet_pool;
void init_config(uint8_t * conf_file,config_t * config)
{
    
}
void init_generator(int numbers)
{
    int i = 0;
    init_config(config_file,config);
    init_pool(PACKET_POOL,config->numbers;sizeof(config->packet_size));
    generator_info.generator = malloc(sizeof(generator_t) * numbers);
    generator_info.numbers   = numbers;
    for(i = 0; i < numbers; ++i)
    {
         
    }
}
void * packet_generator_loop(void * arg)
{
    
}
void generator_worker()
{
    /*
    * 1. get buffer from pool
    * */

    /*
    * 2. 根据配置文件比如UDP，TCP来产生包结构。
    * */

    /*
    * 3. 数据放到下一步的队列里。
    * */
}
