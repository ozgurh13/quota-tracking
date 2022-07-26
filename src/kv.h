#ifndef __KV_H__
#define __KV_H__

# define  KV_KEY_SIZE       64

typedef struct {
	char key[KV_KEY_SIZE];
	size_t up_value, down_value;
} kv_t;

#endif
