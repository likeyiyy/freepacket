#ifndef L_HASH_H
#define L_HASH_H
#include <sys/types.h>
typedef struct llistitem {
    void *datum;
    struct llistitem *next;
} llistitem;

typedef struct llist {
    llistitem *head;
    unsigned int count;
} llist;

typedef struct bucket {
    llist list;
} bucket_t;

typedef struct hash_table {
    int num_buckets;
    bucket_t *buckets;
} hash_table;

typedef struct hash_pos {
    int num_buckets;
    int curr;
    bucket_t *hash_bucket;
    llistitem *ll_item;
    llistitem *ll_last;
} hash_pos;

hash_table *hash_create(int num);
void hash_sizeinfo(unsigned int *sizes, int max, hash_table *ht);
typedef struct hash_item_pid {
    pid_t key;
    void *value;
} hash_item_pid;

void *hash_add_pid(hash_table *ht, pid_t key, void *value);
void *hash_replace_pid(hash_table *ht, pid_t key, void *value);
void *hash_lookup_pid(hash_table *ht, pid_t key);
void *hash_remove_pid(hash_table *ht, pid_t key);
hash_item_pid *hash_first_pid(hash_table *ht, hash_pos *pos);
hash_item_pid *hash_next_pid(hash_pos *pos);
void *hash_remove_pos_pid(hash_pos *pos);

#endif