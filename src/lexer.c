#include "common.h"
#include "lexer.h"
#include "token.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#define NEWLINE(x) (x == '\n' || x == '\r')

static void
lexer_advance (struct lexer *lexer)
{
  lexer->index++;

  lexer->current = lexer->buffer[lexer->index];

  if (lexer->current != '\0')
    lexer->next = lexer->buffer[lexer->index + 1];

  if (NEWLINE (lexer->current))
    lexer->line++;
}

static struct token
lexer_advance_with (struct lexer *lexer, size_t type, size_t amount)
{
  size_t line = lexer->line;

  for (size_t i = amount; i--;)
    lexer_advance (lexer);

  return token_create (NULL, type, line);
}

static char *
lexer_copy_value (struct lexer *lexer, size_t begin)
{
  size_t size = lexer->index - begin;
  char *value;

  value = calloc (size + 1, sizeof (char));
  strncpy (value, &lexer->buffer[begin], size);

  value[size] = '\0';

  return value;
}

static struct token
lexer_parse_number (struct lexer *lexer)
{
  size_t line = lexer->line;
  size_t begin = lexer->index;
  size_t dots = 0;

  while (isdigit (lexer->current) || lexer->current == '.')
    {
      if (lexer->current == '.')
        dots++;
      lexer_advance (lexer);
    }

  if (dots > 1)
    error (lexer->line, "malformed floating-point number");

  char *value = lexer_copy_value (lexer, begin);

  return token_create (value, !dots ? TOKEN_INTEGER : TOKEN_FLOAT, line);
}

static struct token
lexer_parse_string (struct lexer *lexer)
{
  lexer_advance (lexer);

  size_t line = lexer->line;
  size_t begin = lexer->index;

  while (lexer->current != '"')
    {
      if (NEWLINE (lexer->current) || lexer->current == '\0')
        error (lexer->line, "unterminated string-literal");
      lexer_advance (lexer);
    }

  char *value = lexer_copy_value (lexer, begin);

  lexer_advance (lexer);

  return token_create (value, TOKEN_STRING, line);
}

static struct token
lexer_parse_identifier (struct lexer *lexer)
{
  size_t line = lexer->line;
  size_t begin = lexer->index;

  while ((ispunct (lexer->current) || isalnum (lexer->current))
         && strchr ("'()[]{}", lexer->current) == NULL
         && !(lexer->current == ':' && lexer->next == '='))
    lexer_advance (lexer);

  if (begin == lexer->index)
    error (lexer->line, "expected character");

  char *value = lexer_copy_value (lexer, begin);

  return token_create (value, TOKEN_IDENTIFIER, line);
}

struct lexer *
lexer_create (char *buffer)
{
  struct lexer *lexer;

  lexer = calloc (1, sizeof (struct lexer));
  lexer->buffer = buffer;
  lexer->line = 1;

  if (buffer && *buffer != '\0')
    {
      lexer->current = buffer[0];
      lexer->next = buffer[1];
    }

  if (NEWLINE (lexer->current))
    lexer->line++;

  return lexer;
}

void
lexer_destroy (struct lexer *lexer)
{
  free (lexer);
}

struct token
lexer_next (struct lexer *lexer)
{
  for (; lexer->current != '\0'; lexer_advance (lexer))
    {
      if (isblank (lexer->current) || lexer->current == '\n')
        continue;

      if (isdigit (lexer->current))
        return lexer_parse_number (lexer);
      else if (lexer->current == '"')
        return lexer_parse_string (lexer);
      else if (lexer->current == ':' && lexer->next == '=')
        return lexer_advance_with (lexer, TOKEN_WALRUS, 2);
      else if (lexer->current == '(')
        return lexer_advance_with (lexer, TOKEN_LPAREN, 1);
      else if (lexer->current == ')')
        return lexer_advance_with (lexer, TOKEN_RPAREN, 1);
      else if (lexer->current == '[')
        return lexer_advance_with (lexer, TOKEN_LBRACKET, 1);
      else if (lexer->current == ']')
        return lexer_advance_with (lexer, TOKEN_RBRACKET, 1);
      else if (lexer->current == '{')
        return lexer_advance_with (lexer, TOKEN_LBRACE, 1);
      else if (lexer->current == '}')
        return lexer_advance_with (lexer, TOKEN_RBRACE, 1);
      else if (ispunct (lexer->current) || isalpha (lexer->current))
        {
          bool symbol = false;
          struct token identifier;

          if ((symbol = lexer->current == '\''))
            lexer_advance (lexer);

          identifier = lexer_parse_identifier (lexer);
          identifier.type = symbol ? TOKEN_SYMBOL : identifier.type;

          return identifier;
        }

      error (lexer->line, "unexpected character `%c`", lexer->current);
    }

  return token_create (NULL, TOKEN_EOF, lexer->line);
}

struct token
lexer_peek (struct lexer *lexer)
{
  size_t index = lexer->index;
  size_t line = lexer->line;
  bool newline = NEWLINE (lexer->current);

  struct token token = lexer_next (lexer);

  lexer->index = index - 1;
  lexer->line = line - newline;
  lexer_advance (lexer);

  return token;
}

