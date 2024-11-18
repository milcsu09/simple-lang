#include "lexer.h"
#include "common.h"
#include "token.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#define IS_NEWLINE(ch) ((ch) == '\n' || (ch) == '\r')
#define IS_WHITESPACE(ch) (isblank (ch) || IS_NEWLINE (ch))
#define IS_SPECIAL(ch) (strchr ("'\"=()[]{}", ch) != NULL)

static void
lexer_advance (struct lexer *lexer)
{
  lexer->index++;
  lexer->current = lexer->buffer[lexer->index];

  if (lexer->current != '\0')
    lexer->next = lexer->buffer[lexer->index + 1];

  if (IS_NEWLINE (lexer->current))
    lexer->line++;
}

static struct token
lexer_advance_with (struct lexer *lexer, size_t type, size_t advance)
{
  size_t line = lexer->line;

  while (advance--)
    lexer_advance (lexer);

  return token_create (NULL, type, line);
}

static char *
lexer_copy_value (struct lexer *lexer, size_t begin)
{
  size_t size = lexer->index - begin;
  char *value;

  value = calloc (size + 1, sizeof (char));
  memcpy (value, &lexer->buffer[begin], size);

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
      if (lexer->current == '.' && ++dots > 1)
        error (line, "malformed floating-point number");
      lexer_advance (lexer);
    }

  char *value = lexer_copy_value (lexer, begin);
  size_t type = !dots ? TOKEN_INTEGER : TOKEN_FLOAT;

  return token_create (value, type, line);
}

static struct token
lexer_parse_string (struct lexer *lexer)
{
  lexer_advance (lexer);

  size_t line = lexer->line;
  size_t begin = lexer->index;

  while (lexer->current != '"')
    {
      if (IS_NEWLINE (lexer->current) || lexer->current == '\0')
        error (line, "unterminated string-literal");
      lexer_advance (lexer);
    }

  char *value = lexer_copy_value (lexer, begin);

  lexer_advance (lexer);

  return token_create (value, TOKEN_STRING, line);
}

static struct token
lexer_parse_word (struct lexer *lexer)
{
  bool symbol = false;
  if ((symbol = lexer->current == '\''))
    lexer_advance (lexer);

  size_t line = lexer->line;
  size_t begin = lexer->index;

  while ((ispunct (lexer->current) || isalnum (lexer->current))
         && !(lexer->current == '=' && lexer->next == '>')
         && !IS_SPECIAL (lexer->current))
    lexer_advance (lexer);

  if (begin == lexer->index)
    error (line, "expected character");

  char *value = lexer_copy_value (lexer, begin);
  size_t type = !symbol ? TOKEN_IDENTIFIER : TOKEN_SYMBOL;

  return token_create (value, type, line);
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

  if (IS_NEWLINE (lexer->current))
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
      if (IS_WHITESPACE (lexer->current))
        continue;

      switch (lexer->current)
        {
        case '(':
          return lexer_advance_with (lexer, TOKEN_LPAREN, 1);
        case ')':
          return lexer_advance_with (lexer, TOKEN_RPAREN, 1);
        case '[':
          return lexer_advance_with (lexer, TOKEN_LBRACKET, 1);
        case ']':
          return lexer_advance_with (lexer, TOKEN_RBRACKET, 1);
        case '{':
          return lexer_advance_with (lexer, TOKEN_LBRACE, 1);
        case '}':
          return lexer_advance_with (lexer, TOKEN_RBRACE, 1);
        case '=':
          if (lexer->next == '>')
            return lexer_advance_with (lexer, TOKEN_ARROW, 2);
          else
            return lexer_advance_with (lexer, TOKEN_EQUALS, 1);
        default:
          if (lexer->current == '-' && lexer->next == '-')
            while (!IS_NEWLINE (lexer->current))
              lexer_advance (lexer);
          else if (lexer->current == '"')
            return lexer_parse_string (lexer);
          else if (isdigit (lexer->current))
            return lexer_parse_number (lexer);
          else if (ispunct (lexer->current) || isalpha (lexer->current))
            return lexer_parse_word (lexer);
          else
            error (lexer->line, "unexpected character `%c`", lexer->current);
        }
    }

  return token_create (NULL, TOKEN_EOF, lexer->line);
}

struct token
lexer_peek (struct lexer *lexer)
{
  size_t index = lexer->index;
  size_t line = lexer->line;
  bool newline = IS_NEWLINE (lexer->current);

  struct token token = lexer_next (lexer);

  lexer->index = index - 1;
  lexer->line = line - newline;

  lexer_advance (lexer);

  return token;
}

