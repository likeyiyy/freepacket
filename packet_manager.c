/*************************************************************************
	> File Name: session.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 12 May 2014 10:41:38 AM CST
 ************************************************************************/
#include "includes.h"
/*
* 初始化一个session工作组。
* */
//#define memcpy(a,b,c) do { memcpy(a,b,c);printf("session memcpy here\n"); } while(0)
#define MAKE_HASH(v1,v2,h1,f1,f2,f3,f4,SIZE) \
(\
            v1 = f1 ^ f2,\
            v2 = f3 ^ f4,\
            h1 = v1 << 8,\
            h1 ^= v1 >> 4,\
            h1 ^= v1 >> 12,\
            h1 ^= v1 >> 16,\
            h1 ^= v2 << 6, \
            h1 ^= v2 << 10, \
            h1 ^= v2 << 14, \
            h1 ^= v2 >> 7,\
            h1%SIZE\
)
manager_set_t * manager_set;
static inline int compare_session(session_item_t * item , flow_item_t * flow)
{
    session_item_t * session = item;
    if( session->protocol == flow->protocol &&
        session->upper_ip == flow->upper_ip &&
        session->lower_ip == flow->lower_ip &&
        session->upper_port == flow->upper_port &&
        session->lower_port   == flow->lower_port) 
    {
        return 0;
    }
    return 1;
}
struct blist * find_list(struct list_head * head, flow_item_t * flow)
{
    struct list_head * p;
    struct blist * node;
    list_for_each(p,head)
    {
        node = list_entry(p,struct blist,listhead);
        if(compare_session(&node->item, flow) == 0)
        {
            return node;
        }
    }
    return NULL;
}
void delete_session(hash_table * ht,bucket_t * bucket)
{
    struct list_head * p, * list;
    struct blist * node;
    struct list_head * next;
    uint64_t current_time;
    list = &bucket->list;
    current_time = GET_CYCLE_COUNT();
    list_for_each_safe(p,next,list)
    {
        node = list_entry(p,struct blist,listhead); 
        if((1.0 * node->item.cur_len > MAX_FACTOR * node->item.length) ||
        (current_time - node->item.last_time > DESTORY_TIME))
        {
            list_del(&node->listhead);
            free_buf(node->item.pool,(void *)node); 
            --bucket->count;
        }
    }
}
void * process_session(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    while(1)
    { 
        hash_travel_delete(manager -> ht);
        usleep(1000*1000);
    }
}
/*
* 真正的工作者。
* */
void * packet_manager_loop(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    flow_item_t * flow;
    pthread_t clean_id;
    pthread_create(&clean_id,NULL,process_session,arg);

    uint32_t v1,v2,h1,index;
    while(1)
    {
        /*
         * 1.只有确实有数据时才返回。
         * */
        pop_common_buf(manager->queue,(void **)&flow);

        /*
         * 2. make hash index
         * */

        index = MAKE_HASH(v1,v2,h1,flow->lower_port,
                flow->upper_ip,
                flow->upper_port,
                flow->lower_ip,
                manager->ht->num_buckets);

        /*
        * 3. insert into hash table
        * */
        hash_add_item(&manager->ht, index, flow); 

    }
}
manager_set_t * init_manager_set(sim_config_t * config)
{
    int numbers = config->manager_nums;
    int queue_length = config->manager_queue_length;
    int pool_size    = config->manager_pool_size;
    int hash_length  = config->manager_hash_length;

    manager_set_t * set = malloc(sizeof(manager_set_t));
    manager_set = set;
    exit_if_ptr_is_null(set,"初始化分配流管理错误");
    set->manager = malloc(sizeof(manager_t) * numbers);
    exit_if_ptr_is_null(set->manager,"初始化分配manager错误");
    set->length = numbers;

    session_item_t * session;
    int i = 0;
    for(i = 0; i < numbers; i++)
    {
        set->manager[i].queue = init_common_queue(queue_length,
                sizeof(flow_item_t));
        set->manager[i].ht = hash_create(hash_length);
        set->manager[i].session_pool = init_pool(MANAGER_POOL,
                                                pool_size,
                                                sizeof(struct blist));
        for(int j = 0; j < pool_size - 1; j++)
        {
            get_buf(set->manager[i].session_pool,NO_WAIT_MODE,(void **)&session);
            session->buffer = malloc(config->manager_buffer_size);
            exit_if_ptr_is_null(session->buffer,"session->buffer is NULL");
            free_buf(set->manager[i].session_pool,session);
        }
        set->manager[i].session_pool->pool_type = MANAGER_POOL;
        set->manager[i].index = i;
        set->manager[i].drop_cause_pool_empty = 0;
    }
    for(i = 0; i < numbers; i++)
    {
        pthread_create(&set->manager[i].id,
                       NULL,
                     packet_manager_loop,
                      &set->manager[i]);
    }
    return set;
}
