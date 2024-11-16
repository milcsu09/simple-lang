#ifndef LEXER_H
#define LEXER_H

#include "token.h"

struct lexer
{
  char *buffer;
  size_t index;
  size_t line;
  char current;
  char next;
};

struct lexer *lexer_create (char *buffer);
void lexer_destroy (struct lexer *lexer);

struct token lexer_next (struct lexer *lexer);
struct token lexer_peek (struct lexer *lexer);

#endif // LEXER_H

