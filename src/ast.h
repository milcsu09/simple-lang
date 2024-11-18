#ifndef AST_H
#define AST_H

#include "token.h"

enum
{
  AST_PROGRAM,
  AST_RETURN,

  AST_VARIABLE_DECLARATION,
  AST_FUNCTION_DEFINITION,
  AST_FUNCTION_INVOCATION,
  AST_ARRAY,
  AST_STRUCTURE,

  AST_INTEGER,
  AST_FLOAT,
  AST_STRING,

  AST_IDENTIFIER,
  AST_SYMBOL
};

struct ast
{
  struct token token;
  struct ast *child;
  struct ast *next;
  size_t type;
  size_t line;
};

struct ast *ast_create (size_t type, size_t line);
void ast_destroy (struct ast *node);

void ast_append (struct ast *node, struct ast *child);
const char *ast_type_string (size_t type);

void ast_print_debug (struct ast *node, size_t depth);

#endif // AST_H

