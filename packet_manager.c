/*************************************************************************
	> File Name: session.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Mon 12 May 2014 10:41:38 AM CST
 ************************************************************************/
#include "includes.h"
static manager_group_t * global_manager_group = NULL;
static pthread_t gclean_id;
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
        if((current_time - node->item.last_time) > (global_config->destory_time * 1000UL * global_config->mhz))
        {
            list_del(&node->listhead);
    		while(unlikely(mwsr_mpool_enqueue(node->item.pool,node) != 0))
			{
				continue;	
			}
            --bucket->count;
        }
    }
}
void * process_session(void * arg)
{
    manager_t * manager = (manager_t *)arg;
	uint64_t interval = global_config->destory_time * 1000UL * global_config->mhz;
    while(1)
    { 
		uint64_t start_cycle = GET_CYCLE_COUNT();	
        hash_travel_delete(manager -> ht);
		while(GET_CYCLE_COUNT() < (start_cycle + interval))
			continue;
    }
}

#if 1
static void * process_all_session(void * arg)
{
	manager_group_t * group = (manager_group_t *)arg;
	uint64_t interval = 10000000;
	uint64_t start_cycle = 0;	
	while(1)
	{
		start_cycle = GET_CYCLE_COUNT();	
		for(int i = 0; i < group->numbers; i++)
		{
        	hash_travel_delete(group->manager[i].ht);
		}
		while(GET_CYCLE_COUNT() < (start_cycle + interval))
			continue;
	}
    return NULL;
}
#endif
/*
* 真正的工作者。
* */
#define COUNT 64
void * packet_manager_loop(void * arg)
{
    manager_t * manager = (manager_t *)arg;
    flow_item_t * flow;
    uint32_t v1,v2,h1,index;
	uint64_t start_cycle = 0;	
	start_cycle = GET_CYCLE_COUNT();	
    while(1)
    {
		manager->alive++;
        /*
         * 1.只有确实有数据时才返回。
         * */
        while(unlikely(mwsr_queue_dequeue(manager->queue,(void **)&flow) != 0))
		{
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
    		while(unlikely(mwsr_pool_enqueue(flow->packet->pool,flow->packet) != 0))
			{
				continue;	
			}
    		while(unlikely(mwsr_pool_enqueue(flow->pool,flow) != 0))
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
    struct blist * session;
	struct blist * buffer;

	manager_group->manager[i].queue = memalign(64,sizeof(mwsr_queue_t));
	assert(manager_group->manager[i].queue);
	mwsr_queue_init(manager_group->manager[i].queue);

    manager_group->manager[i].ht = hash_create(hash_length);

	pool_size = (1 << MANAGER_POOL);
    manager_group->manager[i].session_pool = memalign(64, sizeof(mwsr_mpool_t));
	assert(manager_group->manager[i].session_pool);
	mwsr_mpool_init(manager_group->manager[i].session_pool);
	buffer = malloc(pool_size * sizeof(struct blist));
	exit_if_ptr_is_null(buffer,"alloc pool buffer error");
	
    for(int j = 0; j < pool_size; j++)
    {
		session = buffer++;
        session->item.buffer = malloc(global_config->manager_buffer_size);
		mwsr_mpool_enqueue(manager_group->manager[i].session_pool, session);
    }
    manager_group->manager[i].index = i;
    manager_group->manager[i].drop_cause_pool_empty = 0;
	
}
manager_group_t * init_manager_group(sim_config_t * config)
{
    int numbers = config->manager_nums;
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
	if(global_config -> pipe_depth  > 4)
	{
		pthread_create(&gclean_id,NULL,process_all_session,global_manager_group);
	}
    return global_manager_group;
}
manager_group_t * get_manager_group()
{
    return global_manager_group;
}
