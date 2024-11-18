#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum
{
  TYPE_INTEGER,
  TYPE_FLOAT,

  TYPE_STRING,
  TYPE_SYMBOL,

  TYPE_ARRAY,
  TYPE_STRUCTURE,

  TYPE_FUNTION,
  TYPE_NATIVE,

  TYPE_VOID
};

struct value
{
  size_t type;
  size_t refs;
  union
  {
    int i;
    float f;
    void *p;
  };
};

struct value *value_create (size_t type);
void value_destroy (struct value *value);

void value_print (struct value *value, FILE *fd);

bool value_type_match (size_t type, size_t n, ...);
const char *value_type_string (size_t type);

#endif // VALUE_H

