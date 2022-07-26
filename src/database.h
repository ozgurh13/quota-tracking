#ifndef __DATABASE_H__
#define __DATABASE_H__

# include "kv.h"

void db_open();
void db_write(const kv_t kv);

#endif
