#ifndef __HASHMAP_H__
#define __HASHMAP_H__

# include "kv.h"

/*
 * hashing with chaining
 * table doesn't resize, collision are handled by chaining
 */

# define  HASHMAP_TABLE_SIZE     61         // number should be prime

typedef struct node {
	kv_t key;
	struct node *next;
} node_t;

typedef struct {
	node_t* buckets[HASHMAP_TABLE_SIZE];
} hashmap_t;


typedef void (*foreach_t) (node_t *);

void hashmap_init(hashmap_t *hashmap);
void hashmap_insert(hashmap_t *hashmap, kv_t kv);
void hashmap_foreach(hashmap_t *hashmap, foreach_t function);
void hashmap_delete(hashmap_t *hashmap);

#endif
