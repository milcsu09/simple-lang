#include "value.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static const char *const TYPES[] = {
  "INTEGER",
  "FLOAT",
  "STRING",
  "SYMBOL",
  "ARRAY",
  "STRUCTURE",
  "FUNTION",
  "NATIVE",
  "VOID"
};

struct value *
value_create (size_t type)
{
  struct value *value;

  value = calloc (1, sizeof (struct value));
  value->type = type;

  return value;
}

void
value_destroy (struct value *value)
{
  switch (value->type)
    {
    case TYPE_INTEGER:
    case TYPE_FLOAT:
      break;
    case TYPE_STRING:
    case TYPE_SYMBOL:
      free (value->p);
      break;
    case TYPE_ARRAY:
      break;
    case TYPE_STRUCTURE:
      break;

    case TYPE_FUNTION:
      break;
    case TYPE_NATIVE:
      break;

    case TYPE_VOID:
      break;
    }

  free (value);
}

void
value_print (struct value *value, FILE *fd)
{
  switch (value->type)
    {
    case TYPE_INTEGER:
      fprintf (fd, "%i", value->i);
      break;
    case TYPE_FLOAT:
      fprintf (fd, "%g", value->f);
      break;
    case TYPE_STRING:
      fprintf (fd, "\"%s\"", (char *)value->p);
      break;
    case TYPE_SYMBOL:
      fprintf (fd, "'%s", (char *)value->p);
      break;
    case TYPE_ARRAY:
    case TYPE_STRUCTURE:
    case TYPE_FUNTION:
    case TYPE_NATIVE:
    case TYPE_VOID:
      fprintf (fd, "(%s)", value_type_string (value->type));
      break;
    }
}

bool
value_type_match (size_t type, size_t n, ...)
{
  va_list va;
  va_start (va, n);

  for (size_t i = 0; i < n; ++i)
    if (type == va_arg (va, size_t))
      {
        va_end (va);
        return true;
      }

  va_end (va);
  return false;
}

const char *
value_type_string (size_t type)
{
  return TYPES[type];
}


