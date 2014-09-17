/*************************************************************************
	> File Name: session.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 12 May 2014 10:41:38 AM CST
 ************************************************************************/
#include "includes.h"
static manager_group_t * global_manager_group = NULL;
static pthread_mutex_t global_create_manager_lock = PTHREAD_MUTEX_INITIALIZER;
//static pthread_t gclean_id;
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
static inline void free_flow(flow_item_t * flow)
{
    /* 
    * 注意这两个free
    * */
    free_buf(flow->packet->pool,flow->packet);
    free_buf(flow->pool,flow);
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
	uint64_t interval = tmc_perf_get_cpu_speed() ;
    while(1)
    { 
		uint64_t start_cycle = get_cycle_count();	
        hash_travel_delete(manager -> ht);
		while(get_cycle_count() < (start_cycle + interval))
			continue;
    }
}

static void * process_all_session(void * arg)
{
	manager_group_t * group = (manager_group_t *)arg;
	uint64_t interval = tmc_perf_get_cpu_speed() ;
	uint64_t start_cycle = 0;	
	while(1)
	{
		start_cycle = get_cycle_count();	
		for(int i = 0; i < group->numbers; i++)
		{
        	hash_travel_delete(group->manager[i].ht);
		}
		while(get_cycle_count() < (start_cycle + interval))
			continue;
	}
}
/*
* 真正的工作者。
* */
#define COUNT 64
void * packet_manager_loop(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    flow_item_t * flow;
	if(global_config -> pipe_depth  > 4)
	{
    	pthread_t clean_id;
    	pthread_create(&clean_id,NULL,process_session,arg);
	}
    uint32_t v1,v2,h1,index;
	uint64_t interval = tmc_perf_get_cpu_speed() ;
	uint64_t start_cycle = get_cycle_count();	
    while(1)
    {
		manager->alive++;
		//printf("alive ++\n");
        /*
         * 1.只有确实有数据时才返回。
         * */
        while(unlikely(free_queue_dequeue(manager->queue,&flow) != 0))
		{
		manager->alive++;
			continue;
		}
		if(global_config -> pipe_depth  > 4)
		{
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
		else
		{
    		while(unlikely(free_pool_enqueue(flow->packet->pool,flow->packet) != 0))
			{
				continue;	
			}
    		while(unlikely(free_pool_enqueue(flow->pool,flow) != 0))
			{
				continue;	
			}
		}

    }
}
static inline void init_signle_manager(manager_group_t * manager_group,int i)
{
    int pool_size    = global_config->manager_pool_size;
    int hash_length  = global_config->manager_hash_length;
    session_item_t * session;

	manager_group->manager[i].queue = memalign(64,sizeof(free_pool_t));
	assert(manager_group->manager[i].queue);
	free_pool_init(manager_group->manager[i].queue);

    manager_group->manager[i].ht = hash_create(hash_length);
    manager_group->manager[i].session_pool = init_pool(MANAGER_POOL,
                                                pool_size,
                                                sizeof(struct blist));
    for(int j = 0; j < pool_size - 1; j++)
    {
            get_buf(manager_group->manager[i].session_pool,NO_WAIT_MODE,(void **)&session);
            session->buffer = malloc(global_config->manager_buffer_size);
            exit_if_ptr_is_null(session->buffer,"session->buffer is NULL");
            free_buf(manager_group->manager[i].session_pool,session);
    }
    manager_group->manager[i].session_pool->pool_type = MANAGER_POOL;
    manager_group->manager[i].index = i;
    manager_group->manager[i].drop_cause_pool_empty = 0;
	
}
manager_group_t * init_manager_group(sim_config_t * config)
{
    int numbers = config->manager_nums;
    int queue_length = config->manager_queue_length;
    int pool_size    = config->manager_pool_size;
    int hash_length  = config->manager_hash_length;

    global_manager_group = malloc(sizeof(manager_group_t));
    exit_if_ptr_is_null(global_manager_group,"初始化分配流管理错误");
    global_manager_group->manager = malloc(sizeof(manager_t) * numbers);
    exit_if_ptr_is_null(global_manager_group->manager,"初始化分配manager错误");
    global_manager_group->numbers = numbers;
    for(int i = 0; i < numbers; i++)
    {
		init_signle_manager(global_manager_group,i);
        pthread_create(&global_manager_group->manager[i].id,
                       NULL,
                     packet_manager_loop,
                      &global_manager_group->manager[i]);
    }
	//pthread_create(&gclean_id,NULL,process_all_session,global_manager_group);
    return global_manager_group;
}
manager_group_t * get_manager_group()
{
    return global_manager_group;
}
