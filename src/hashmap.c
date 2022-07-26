# include <stdlib.h>
# include <stdbool.h>
# include <stdint.h>
# include <string.h>

# include "hashmap.h"


/*
 * hashmap implementation using hashing with chaining
 *
 * a table of ptrs to linked lists holding the data
 *
 *                   table
 *   +-----+-----+-----+-----+-----+-----+
 *   |  *  |     |  *  |  *  |     |     |
 *   +--+--+-----+--+--+--+--+-----+-----+
 *      |           |     |
 *      |           |     |
 *      v           v     v
 *    +---+       +---+ +---+
 *    |   |       |   | |   |
 *    +---+       +---+ +---+
 *    | * |       |   | |   |
 *    +-+-+       +---+ +---+
 *      |
 *      |
 *      v
 *    +---+
 *    |   |
 *    +---+
 *    |   |
 *    +---+
 */



static node_t*
node_create(kv_t kv)
{
	node_t *node = malloc(sizeof(node_t));
	node->key = kv;
	node->next = NULL;
	return node;
}

static uint32_t
get_hash(char* string)
{
	uint32_t hash = 0;
	for (char *c = string; *c != '\0'; c++)
		hash = (uint32_t) *c + (hash << 6) + (hash << 16) - hash;
	return hash;
}


void
hashmap_init(hashmap_t *hashmap)
{
	for (int i = 0; i < HASHMAP_TABLE_SIZE; i++)
		hashmap->buckets[i] = NULL;
}

void
hashmap_insert(hashmap_t *hashmap, kv_t kv)
{
	const uint32_t hash = get_hash(kv.key) % HASHMAP_TABLE_SIZE;
	node_t *node = hashmap->buckets[hash];

	if (node == NULL)    // if bucket is empty
		hashmap->buckets[hash] = node_create(kv);
	else {               // if key exists update, else insert
		while (true) {
			if (strcmp(node->key.key, kv.key) == 0) {      // if key exists
				node->key.up_value   += kv.up_value;
				node->key.down_value += kv.down_value;
				return;
			}

			if (node->next == NULL) {                      // if we are at the end
				node->next = node_create(kv);
				return;
			}

			node = node->next;
		}
	}
}

void
hashmap_foreach(hashmap_t *hashmap, foreach_t function)
{
	for (int i = 0; i < HASHMAP_TABLE_SIZE; i++)
		for (node_t *node = hashmap->buckets[i]; node != NULL; node = node->next)
			function(node);
}


static void
list_delete(node_t *node)
{
	node_t *tmp;
	while (node != NULL) {
		tmp = node;
		node = node->next;
		free(tmp);
	}
}

void
hashmap_delete(hashmap_t *hashmap)
{
	for (int i = 0; i < HASHMAP_TABLE_SIZE; i++) {
		list_delete(hashmap->buckets[i]);
		hashmap->buckets[i] = NULL;
	}
}

