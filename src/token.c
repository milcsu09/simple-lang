#include "token.h"
#include <stdarg.h>
#include <stdlib.h>

static const char *const TYPES[] = {
  "INTEGER",
  "FLOAT",
  "STRING",
  "IDENTIFIER",
  "SYMBOL",
  "ARROW",
  "WALRUS",
  "LPAREN",
  "RPAREN",
  "LBRACKET",
  "RBRACKET",
  "LBRACE",
  "RBRACE",
  "EOF"
};

struct token
token_create (char *value, size_t type, size_t line)
{
  struct token token;

  token.value = value;
  token.type = type;
  token.line = line;

  return token;
}

void
token_destroy (struct token token)
{
  free (token.value);
  token.value = NULL;
}

bool
token_type_match (size_t type, size_t n, ...)
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
token_type_string (size_t type)
{
  return TYPES[type];
}

