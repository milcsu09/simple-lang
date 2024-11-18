#include "tables.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

static size_t
hash (const char *key, size_t capacity)
{
  size_t hash = 5381;
  char c;

  while ((c = *key++))
    hash = ((hash << 5) + hash) + c;

  return hash % capacity;
}

static struct bucket *
bucket_create (const char *key, struct value *value)
{
  struct bucket *bucket;

  bucket = calloc (1, sizeof (struct bucket));
  bucket->key = xstrdup (key);
  bucket->value = value;

  return bucket;
}

static void
bucket_destroy (struct bucket *bucket)
{
  free (bucket->key);
  free (bucket);
}

static void
hash_table_resize (struct hash_table *table)
{
  struct bucket **old_buckets = table->buckets;
  size_t old_capacity = table->capacity;

  table->capacity = table->capacity * 2;
  table->buckets = calloc (table->capacity, sizeof (struct bucket *));
  table->length = 0;

  for (size_t i = 0; i < old_capacity; ++i)
    {
      struct bucket *current = old_buckets[i];

      while (current != NULL)
        {
          hash_table_append (table, current->key, current->value);

          struct bucket *previous = current;
          current = current->next;

          bucket_destroy (previous);
        }
    }

  free (old_buckets);
}

struct hash_table *
hash_table_create (size_t capacity)
{
  struct hash_table *table;

  table = calloc (1, sizeof (struct hash_table));
  table->buckets = calloc (capacity, sizeof (struct bucket *));
  table->capacity = capacity;

  return table;
}

void
hash_table_destroy (struct hash_table *table)
{
  for (size_t i = 0; i < table->capacity; ++i)
    {
      struct bucket *current = table->buckets[i];

      while (current != NULL)
        {
          struct bucket *previous = current;
          current = current->next;

          bucket_destroy (previous);
        }
    }

  free (table->buckets);
  free (table);
}

void
hash_table_append (struct hash_table *table, const char *key,
                   struct value *value)
{
  size_t index = hash (key, table->capacity);
  struct bucket *current = table->buckets[index];

  while (current != NULL)
    if (strcmp (current->key, key) != 0)
      current = current->next;
    else
      {
        current->value = value;
        return;
      }
 
  struct bucket *new = bucket_create (key, value);

  new->next = table->buckets[index];
  table->buckets[index] = new;

  table->length++;

  if ((float)table->length / table->capacity > 0.75)
    hash_table_resize (table);
}

struct value *
hash_table_find (struct hash_table *table, const char *key)
{
  size_t index = hash (key, table->capacity);
  struct bucket *current = table->buckets[index];

  while (current != NULL)
    {
      if (strcmp (current->key, key) == 0)
        return current->value;

      current = current->next;
    }

  return NULL;
}

