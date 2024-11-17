#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stddef.h>

enum
{
  TOKEN_INTEGER,
  TOKEN_FLOAT,
  TOKEN_STRING,

  TOKEN_IDENTIFIER,
  TOKEN_SYMBOL,

  TOKEN_ARROW,
  TOKEN_WALRUS,

  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LBRACE,
  TOKEN_RBRACE,

  TOKEN_EOF
};

struct token
{
  char *value;
  size_t type;
  size_t line;
};

struct token token_create (char *value, size_t type, size_t line);
void token_destroy (struct token token);

bool token_type_match (size_t type, size_t n, ...);
const char *token_type_string (size_t type);

#endif // TOKEN_H

