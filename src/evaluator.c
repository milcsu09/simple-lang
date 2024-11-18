#include "evaluator.h"
#include "common.h"
#include <stdlib.h>

struct value *evaluate_program (struct ast *node);
struct value *evaluate_return (struct ast *node);
struct value *evaluate_variable_declaration (struct ast *node);
struct value *evaluate_function_definition (struct ast *node);
struct value *evaluate_function_invocation (struct ast *node);
struct value *evaluate_array (struct ast *node);
struct value *evaluate_structure (struct ast *node);
struct value *evaluate_integer (struct ast *node);
struct value *evaluate_float (struct ast *node);
struct value *evaluate_string (struct ast *node);
struct value *evaluate_identifier (struct ast *node);
struct value *evaluate_symbol (struct ast *node);

struct value *
evaluate_program (struct ast *node)
{
  struct ast *current = node->child;

  while (current != NULL)
    {
      struct value *value = evaluate (current);

      if (current->type == AST_RETURN)
        return value;

      if (value->refs == 0)
        value_destroy (value);

      current = current->next;
    }

  return value_create (TYPE_VOID);
}

struct value *
evaluate_return (struct ast *node)
{
  return evaluate (node->child);
}

struct value *
evaluate_integer (struct ast *node)
{
  struct value *value;

  value = value_create (TYPE_INTEGER);
  value->i = atoi (node->token.value);

  return value;
}

struct value *
evaluate_float (struct ast *node)
{
  struct value *value;

  value = value_create (TYPE_FLOAT);
  value->f = atof (node->token.value);

  return value;
}

struct value *
evaluate_string (struct ast *node)
{
  struct value *value;

  value = value_create (TYPE_STRING);
  value->p = xstrdup (node->token.value);

  return value;
}

struct value *
evaluate_symbol (struct ast *node)
{
  struct value *value;

  value = value_create (TYPE_SYMBOL);
  value->p = xstrdup (node->token.value);

  return value;
}

struct value *
evaluate (struct ast *node)
{
  switch (node->type)
    {
    case AST_PROGRAM:
      return evaluate_program (node);
    case AST_RETURN:
      return evaluate_return (node);
    case AST_VARIABLE_DECLARATION:
      break;
    case AST_FUNCTION_DEFINITION:
      break;
    case AST_FUNCTION_INVOCATION:
      break;
    case AST_ARRAY:
      break;
    case AST_STRUCTURE:
      break;
    case AST_INTEGER:
      return evaluate_integer (node);
    case AST_FLOAT:
      return evaluate_float (node);
    case AST_STRING:
      return evaluate_string (node);
    case AST_IDENTIFIER:
      break;
    case AST_SYMBOL:
      return evaluate_symbol (node);
    }

  error (node->line, "`evaluate ()` cannot handle `%s` node",
         ast_type_string (node->type));

  // return NULL;
}

