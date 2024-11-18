#ifndef TABLES_H
#define TABLES_H

#include "value.h"

struct bucket
{
  struct bucket *next;
  char *key;
  struct value *value;
};

struct hash_table
{
  struct bucket **buckets;
  size_t length;
  size_t capacity;
};

struct hash_table *hash_table_create (size_t capacity);
void hash_table_destroy (struct hash_table *table);

void hash_table_append (struct hash_table *table, const char *key,
                        struct value *value);
struct value *hash_table_find (struct hash_table *table, const char *key);

#endif // TABLES_H

