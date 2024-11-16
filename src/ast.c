#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

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

static const char *const TYPES[] = { "PROGRAM",
                                     "RETURN",
                                     "VARIABLE_DECLARATION",
                                     "FUNCTION_DEFINITION",
                                     "FUNCTION_INVOCATION",
                                     "ARRAY",
                                     "STRUCT",
                                     "INTEGER",
                                     "FLOAT",
                                     "STRING",
                                     "IDENTIFIER",
                                     "SYMBOL" };

void
ast_print_debug (struct ast *node, size_t depth)
{
  if (node == NULL)
    return;

  for (size_t i = depth * 4; i--;)
    printf (" ");

  printf ("%s:", TYPES[node->type]);

  if (node->token.value)
    printf (" `%s`", node->token.value);

  printf (" (line %zu)", node->line);
  printf ("\n");

  if (node->child != NULL)
    ast_print_debug (node->child, depth + 1);

  if (node->next != NULL)
    ast_print_debug (node->next, depth);
}

