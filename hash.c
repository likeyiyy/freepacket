/*************************************************************************
	> File Name: hash.c
	> Author: likeyi
	> Mail: likeyi@sina.com 
	> Created Time: Tue 17 Jun 2014 04:36:47 PM CST
 ************************************************************************/

#include "includes.h"

  
static int next_prime(int x)
{
     long i, j;
     int f;
     
     x = (x==0)?1:x;
     i = x;
     while (i++)
     {
          f=1;
          for (j=2; j<i; j++)
          {
               if (i%j == 0)
               {
                    f=0;
                    break;
                    
               }
          }
          if (f)
          {
             return (int)i;
          }
     }
    return 0;
}

/*
* 1. 创建
* */
hash_table * hash_create(int num)
{
    hash_table * result;
    bucket_t * b;
    int bytes;
    int i;
    result = malloc(sizeof(hash_table));
    exit_if_ptr_is_null(result,"Initizial hash table Error"); 
    num = next_prime(num);

    bytes = num * sizeof(bucket_t);

    result->buckets = b = malloc(bytes);
    exit_if_ptr_is_null(result->buckets,"hash table buckets alloc error");

    result->num_buckets = num;

    i = num;
    while(--i >= 0)
    {
        INIT_LIST_HEAD(&b->list); 
        pthread_mutex_init(&b->lock, NULL);
        b->count = 0;
        b++;
    }
    return result;
}
/*
* 2. 查找
* */
void * hash_lookup_item(hash_table * ht, uint32_t key, void * value)
{
    struct blist * list = (struct blist *)value;
    struct list_head * ll;
    session_item_t * session = &list->item;

    bucket_t * bucket = &ht -> buckets[key % ht->num_buckets];
    ll = &bucket->list;
    return (void *) find_list(ll,(flow_item_t *)session);    
}
static inline void make_new_session(struct blist * blist,
                                    flow_item_t * flow,
                                    manager_t * manager)
{
    session_item_t * item = &blist->item;
    item->pool       = manager->session_pool;
    item->length     = global_config->manager_buffer_size;
    item->cur_len    = 0;
    item->upper_ip   = flow->upper_ip;
    item->lower_ip   = flow->lower_ip;
    item->upper_port = flow->upper_port;
    item->lower_port = flow->lower_port;
    item->protocol   = flow->protocol;
    item->last_time  = GET_CYCLE_COUNT();
    memcpy(item->buffer,flow->payload,flow->payload_len);
    item->cur_len  += flow->payload_len;
}
/*
* 3. 插入
* 严格说来,blist 完全没有任何问题。
* */
typedef session_item_t hash_item_t ;
int  hash_add_item(hash_table ** htp, uint32_t key, void * value )
{
    struct list_head * ll;
    struct blist * blist;
    struct blist * new_blist;
    hash_table * ht = *htp;
    flow_item_t * flow = (flow_item_t *)value;
    manager_t * manager = list_entry(htp,manager_t,ht);

    /*
     * 
     * */
    bucket_t * bucket = &ht -> buckets[key % ht->num_buckets];

    pthread_mutex_lock(&bucket->lock);

    ll = &bucket->list;

    blist = find_list(ll,flow);    
    /*
     * 假如不存在于链表中。
     * */
    if(!blist)
    {
        while(unlikely(mwsr_mpool_dequeue(manager->session_pool,(void **)&new_blist) != 0))
		{
			continue;
		}
        /*
         * 把flow里面的数据拷贝到session里面
         * */
        make_new_session(new_blist, flow, manager);
        /*
         * 立即释放掉
         * */
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
        /*
         * 初始化，并且添加到新建的列表里。
         * */
        INIT_LIST_HEAD(&new_blist->listhead);
        list_add_tail(&new_blist->listhead,ll);
        ++bucket->count;
    }
    /* 
    * Found it, and memcpy it.
    * 主要是这一段无法公共化，其他的hash函数可能找到后，并不会copy
    * */
    else
    {
        hash_item_t * session = &blist->item;
        if(session->cur_len + flow->payload_len < session->length)
        {
            memcpy(session->buffer + session->cur_len, 
                    flow->payload,
                    flow->payload_len);
            session->cur_len += flow->payload_len;
            session->last_time = GET_CYCLE_COUNT();
        }
        else
        {
            /*
             * 计算CRC，并且释放掉条目
             * */

#if 1
            crc32_32((uint32_t*)session->buffer,session->cur_len / 4);
            list_del(&blist->listhead);
    		while(unlikely(mwsr_mpool_enqueue(blist->item.pool,blist) != 0))
			{
				continue;	
			}
            --bucket->count;
#endif
        }
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
    pthread_mutex_unlock(&bucket->lock);
    return 0;
}
/*
* 4. 遍历
* */

void  hash_travel_delete(hash_table * ht)
{
    register int i = 0;
    register bucket_t * bucket;
    bucket = ht->buckets;
    while(i++ < ht->num_buckets)
    {

        pthread_mutex_lock(&bucket->lock);
        delete_session(ht,bucket);
        pthread_mutex_unlock(&bucket->lock);
        bucket++;
    }
}



/*
 * unsigned int hash_count(hash_table *ht)
 *
 * Return total number of elements contained in hash table.
 */

uint32_t hash_count(hash_table * ht)

{
    register int i = 0;
    register int cnt = 0;
    register bucket_t *bucket;

    bucket = ht->buckets;
    while (i++ < ht->num_buckets)
    {
	    cnt += bucket->count;
	    bucket++;
    }

    return cnt;
}
