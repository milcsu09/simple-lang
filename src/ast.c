#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

static const char *const TYPES[] = {
  "PROGRAM",
  "RETURN",
  "VARIABLE_DECLARATION",
  "FUNCTION_DEFINITION",
  "FUNCTION_INVOCATION",
  "ARRAY",
  "STRUCTURE",
  "INTEGER",
  "FLOAT",
  "STRING",
  "IDENTIFIER",
  "SYMBOL"
};

struct ast *
ast_create (size_t type, size_t line)
{
  struct ast *node;

  node = calloc (1, sizeof (struct ast));
  node->type = type;
  node->line = line;

  return node;
}

void
ast_destroy (struct ast *node)
{
  if (node->child != NULL)
    ast_destroy (node->child);

  if (node->next != NULL)
    ast_destroy (node->next);

  if (node->token.value != NULL)
    token_destroy (node->token);

  free (node);
}

void
ast_push (struct ast *node, struct ast *child)
{
  if (node->child == NULL)
    node->child = child;
  else
    {
      struct ast *current;

      current = node->child;
      while (current->next != NULL)
        current = current->next;
      current->next = child;
    }
}

const char *
ast_type_string (size_t type)
{
  return TYPES[type];
}

void
ast_print_debug (struct ast *node, size_t depth)
{
  if (node == NULL)
    return;

  for (size_t i = depth * 4; i--;)
    printf (" ");

  printf ("%s:", ast_type_string (node->type));

  if (node->token.value)
    printf (" `%s`", node->token.value);

  printf (" (line %zu)", node->line);
  printf ("\n");

  if (node->child != NULL)
    ast_print_debug (node->child, depth + 1);

  if (node->next != NULL)
    ast_print_debug (node->next, depth);
}

