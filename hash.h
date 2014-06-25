/*************************************************************************
	> File Name: hash.h
	> Author: likeyi
	> Mail: likeyi@sina.com 
	> Created Time: Tue 17 Jun 2014 04:36:40 PM CST
 ************************************************************************/

#ifndef L_HASH_H
#define L_HASH_H

typedef struct bucket { 
    struct list_head list;
    pthread_mutex_t  lock;
    uint32_t count;
} bucket_t; 
               
typedef struct hash_table {
        int num_buckets;
        bucket_t *buckets;
} hash_table;

/*
* 1. 创建
* */
hash_table * hash_create(int num);
/*
* 2. 插入
* */
void  hash_add_item(hash_table * ht,uint32_t key, void * value);
/*
* 3. 查找
* */
void * hash_lookup_item(hash_table * ht,uint32_t key, void * value);
/*
* 4. 遍历并且删除不符合条件的.
* */
void   hash_travel_delete(hash_table * ht);
uint32_t hash_count(hash_table *ht);
#endif
