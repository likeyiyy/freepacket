#include "top_config.h"

#if STDC_HEADERS
#include <string.h>
#include <stdlib.h>
#define memzero(a, b)		memset((a), 0, (b))
#else /* !STDC_HEADERS */
#ifdef HAVE_MEMCPY
#define memzero(a, b)		memset((a), 0, (b))
#else
#define memcpy(a, b, c)		memcpy(a, b, c)		
#define memzero(a, b)		bzero((a), (b))
#define memcmp(a, b, c)		bcmp((a), (b), (c))
#endif /* HAVE_MEMCPY */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#else
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#endif
void *malloc();
void free();
char *strdup();
#endif /* !STDC_HEADERS */

/* After all that there are still some systems that don't have NULL defined */
#ifndef NULL
#define NULL 0
#endif

#ifdef HAVE_MATH_H
#include <math.h>
#endif


#include "hash.h"
#include <math.h>





static int
next_prime(int x)

{
    long i, j;
    int f;

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
    return 1;
}


void ll_init(llist *q)

{
    q->head = NULL;
    q->count = 0;
}

llistitem *ll_newitem(int size)

{
    llistitem *qi;

    qi = (llistitem *)malloc(sizeof(llistitem) + size);
    qi->datum = ((void *)qi + sizeof(llistitem));
    return qi;
}

void ll_freeitem(llistitem *li)

{
    free(li);
}

void ll_add(llist *q, llistitem *new)

{
    new->next = q->head;
    q->head = new;
    q->count++;
}

void ll_extract(llist *q, llistitem *qi, llistitem *last)

{
    if (last == NULL)
    {
	q->head = qi->next;
    }
    else
    {
	last->next = qi->next;
    }
    qi->next = NULL;
    q->count--;
}

#define LL_FIRST(q) ((q)->head)
llistitem *
ll_first(llist *q)

{
    return q->head;
}

#define LL_NEXT(q, qi)  ((qi) != NULL ? (qi)->next : NULL)
llistitem *
ll_next(llist *q, llistitem *qi)

{
    return (qi != NULL ? qi->next : NULL);
}

#define LL_ISEMPTY(ll)  ((ll)->count == 0)
int
ll_isempty(llist *ll)

{
    return (ll->count == 0);
}

/*
 * hash_table *hash_create(int num)
 *
 * Creates a hash table structure with at least "num" buckets.
 */

hash_table * hash_create(int num)

{
    hash_table *result;
    bucket_t *b;
    int bytes;
    int i;

    /* create the resultant structure */
    result = (hash_table *)malloc(sizeof(hash_table));

    /* adjust bucket count to be prime */
    num = next_prime(num);

    /* create the buckets */
    bytes = sizeof(bucket_t) * num;
    result->buckets = b = (bucket_t *)malloc(bytes);
    result->num_buckets = num;

    /* create each bucket as a linked list */
    i = num;
    while (--i >= 0)
    {
	ll_init(&(b->list));
	b++;
    }

    return result;
}

/*
 * unsigned int hash_count(hash_table *ht)
 *
 * Return total number of elements contained in hash table.
 */

unsigned int
hash_count(hash_table *ht)

{
    register int i = 0;
    register int cnt = 0;
    register bucket_t *bucket;

    bucket = ht->buckets;
    while (i++ < ht->num_buckets)
    {
	cnt += bucket->list.count;
	bucket++;
    }

    return cnt;
}

/*
 * void hash_sizeinfo(unsigned int *sizes, int max, hash_table *ht)
 *
 * Fill in "sizes" with information about bucket lengths in hash
 * table "max".
 */

void 
hash_sizeinfo(unsigned int *sizes, int max, hash_table *ht)

{
    register int i;
    register int idx;
    register bucket_t *bucket;

    memzero(sizes, max * sizeof(unsigned int));

    bucket = ht->buckets;
    i = 0;
    while (i++ < ht->num_buckets)
    {
	idx = bucket->list.count;
	sizes[idx >= max ? max-1 : idx]++;
	bucket++;
    }
}

/*
 * void hash_add_pid(hash_table *ht, pid_t key, void *value)
 *
 * Add an element to table "ht".  The element is described by
 * "key" and "value".  Return NULL on success.  If the key
 * already exists in the table, then no action is taken and
 * the data for the existing element is returned.
 * Key type is pid_t
 */

void *
hash_add_pid(hash_table *ht, pid_t key, void *value)

{
    bucket_t *bucket;
    hash_item_pid *hi;
    hash_item_pid *h;
    llist *ll;
    llistitem *li;
    llistitem *newli;
    pid_t k1;

    /* allocate the space we will need */
    newli = ll_newitem(sizeof(hash_item_pid));
    hi = (hash_item_pid *)newli->datum;

    /* fill in the values */
    hi->key = key;
    hi->value = value;

    /* hash to the bucket */
    bucket = &(ht->buckets[(key % ht->num_buckets)]);

    /* walk the list to make sure we do not have a duplicate */
    ll = &(bucket->list);
    li = LL_FIRST(ll);
    while (li != NULL)
    {
	    h = (hash_item_pid *)li->datum;
	    k1 = h->key;
	    if (key == k1)
	    {
	        /* found one */
	        break;
	    }
	    li = LL_NEXT(ll, li);
    }
    li = NULL;

    /* is there already one there? */
    if (li == NULL)
    {
	/* add the unique element to the buckets list */
	    ll_add(&(bucket->list), newli);
	    return NULL;
    }
    else
    {
	    /* free the stuff we just allocated */
	    ll_freeitem(newli);
	    return ((hash_item_pid *)(li->datum))->value;
    }
}

/*
 * void *hash_replace_pid(hash_table *ht, pid_t key, void *value)
 *
 * Replace an existing value in the hash table with a new one and
 * return the old value.  If the key does not already exist in
 * the hash table, add a new element and return NULL.
 * Key type is pid_t
 */

void *
hash_replace_pid(hash_table *ht, pid_t key, void *value)

{
    bucket_t *bucket;
    hash_item_pid *hi;
    llist *ll;
    llistitem *li;
    void *result = NULL;
    pid_t k1;

    /* find the bucket */
    bucket = &(ht->buckets[(key % ht->num_buckets)]);

    /* walk the list until we find the existing item */
    ll = &(bucket->list);
    li = LL_FIRST(ll);
    while (li != NULL)
    {
	hi = (hash_item_pid *)li->datum;
	k1 = hi->key;
	if (key == k1)
	{
	    /* found it: now replace the value with the new one */
	    result = hi->value;
	    hi->value = value;
	    break;
	}
	li = LL_NEXT(ll, li);
    }

    /* if the element wasnt found, add it */
    if (result == NULL)
    {
	li = ll_newitem(sizeof(hash_item_pid));
	hi = (hash_item_pid *)li->datum;
	hi->key = key;
	hi->value = value;
	ll_add(&(bucket->list), li);
    }

    /* return the old value (so it can be freed) */
    return result;
}

/*
 * void *hash_lookup_pid(hash_table *ht, pid_t key)
 *
 * Look up "key" in "ht" and return the associated value.  If "key"
 * is not found, return NULL.  Key type is pid_t
 */

void *
hash_lookup_pid(hash_table *ht, pid_t key)

{
    bucket_t *bucket;
    llist *ll;
    llistitem *li;
    hash_item_pid *h;
    void *result;
    pid_t k1;

    result = NULL;
    if ((bucket = &(ht->buckets[(key % ht->num_buckets)])) != NULL)
    {
	ll = &(bucket->list);
	li = LL_FIRST(ll);
	while (li != NULL)
	{
	    h = (hash_item_pid *)li->datum;
	    k1 = h->key;
	    if (key == k1)
	    {
		result = h->value;
		break;
	    }
	    li = LL_NEXT(ll, li);
	}
    }
    return result;
}

/*
 * void *hash_remove_pid(hash_table *ht, pid_t key)
 *
 * Remove the element associated with "key" from the hash table
 * "ht".  Return the value or NULL if the key was not found.
 */

void *
hash_remove_pid(hash_table *ht, pid_t key)

{
    bucket_t *bucket;
    llist *ll;
    llistitem *li;
    llistitem *lilast;
    hash_item_pid *hi;
    void *result;
    pid_t k1;

    result = NULL;
    if ((bucket = &(ht->buckets[(key % ht->num_buckets)])) != NULL)
    {
	ll = &(bucket->list);
	li = LL_FIRST(ll);
	lilast = NULL;
	while (li != NULL)
	{
	    hi = (hash_item_pid *)li->datum;
	    k1 = hi->key;
	    if (key == k1)
	    {
		ll_extract(ll, li, lilast);
		result = hi->value;
		key = hi->key;
		;
		ll_freeitem(li);
		break;
	    }
	    lilast = li;
	    li = LL_NEXT(ll, li);
	}
    }
    return result;
}

/*
 * hash_item_pid *hash_first_pid(hash_table *ht, hash_pos *pos)
 *
 * First function to call when iterating through all items in the hash
 * table.  Returns the first item in "ht" and initializes "*pos" to track
 * the current position.
 */

hash_item_pid *
hash_first_pid(hash_table *ht, hash_pos *pos)

{
    /* initialize pos for first item in first bucket */
    pos->num_buckets = ht->num_buckets;
    pos->hash_bucket = ht->buckets;
    pos->curr = 0;
    pos->ll_last = NULL;

    /* find the first non-empty bucket */
    while(pos->hash_bucket->list.count == 0)
    {
	pos->hash_bucket++;
	if (++pos->curr >= pos->num_buckets)
	{
	    return NULL;
	}
    }

    /* set and return the first item */
    pos->ll_item = LL_FIRST(&(pos->hash_bucket->list));
    return (hash_item_pid *)pos->ll_item->datum;
}


/*
 * hash_item_pid *hash_next_pid(hash_pos *pos)
 *
 * Return the next item in the hash table, using "pos" as a description
 * of the present position in the hash table.  "pos" also identifies the
 * specific hash table.  Return NULL if there are no more elements.
 */

hash_item_pid *
hash_next_pid(hash_pos *pos)

{
    llistitem *li;

    /* move item to last and check for NULL */
    if ((pos->ll_last = pos->ll_item) == NULL)
    {
	/* are we really at the end of the hash table? */
	if (pos->curr >= pos->num_buckets)
	{
	    /* yes: return NULL */
	    return NULL;
	}
	/* no: regrab first item in current bucket list (might be NULL) */
	li = LL_FIRST(&(pos->hash_bucket->list));
    }
    else
    {
	/* get the next item in the llist */
	li = LL_NEXT(&(pos->hash_bucket->list), pos->ll_item);
    }

    /* if its NULL we have to find another bucket */
    while (li == NULL)
    {
	/* locate another bucket */
	pos->ll_last = NULL;

	/* move to the next one */
	pos->hash_bucket++;
	if (++pos->curr >= pos->num_buckets)
	{
	    /* at the end of the hash table */
	    pos->ll_item = NULL;
	    return NULL;
	}

	/* get the first element (might be NULL) */
	li = LL_FIRST(&(pos->hash_bucket->list));
    }

    /* li is the next element to dish out */
    pos->ll_item = li;
    return (hash_item_pid *)li->datum;
}

/*
 * void *hash_remove_pos_pid(hash_pos *pos)
 *
 * Remove the hash table entry pointed to by position marker "pos".
 * The data from the entry is returned upon success, otherwise NULL.
 */


void *
hash_remove_pos_pid(hash_pos *pos)

{
    llistitem *li;
    void *ans;
    hash_item_pid *hi;
    pid_t key;

    /* sanity checks */
    if (pos == NULL || pos->ll_last == pos->ll_item)
    {
	return NULL;
    }

    /* at this point pos contains the item to remove (ll_item)
       and its predecesor (ll_last) */
    /* extract the item from the llist */
    li = pos->ll_item;
    ll_extract(&(pos->hash_bucket->list), li, pos->ll_last);

    /* retain the data */
    hi = (hash_item_pid *)li->datum;
    ans = hi->value;

    /* free up the space */
    key = hi->key;
    ;
    ll_freeitem(li);

    /* back up to previous item */
    /* its okay for ll_item to be null: hash_next will detect it */
    pos->ll_item = pos->ll_last;

    return ans;
}


