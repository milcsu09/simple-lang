#include "lexer.h"
#include "parser.h"
#include "array.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
  TYPE_INTEGER,
  TYPE_FLOAT,
  TYPE_STRING,
  TYPE_SYMBOL,

  TYPE_ARRAY,
  TYPE_STRUCT,

  TYPE_FUNTION,
  TYPE_NATIVE,

  TYPE_VOID
};

struct value
{
  union
  {
    int v_integer;
    float v_float;
    char *v_string;
  };
  size_t type;
  size_t refs;
};

static struct value *
value_create (size_t type)
{
  struct value *value;

  value = calloc (1, sizeof (struct value));
  value->type = type;

  return value;
}

static void
value_destroy (struct value *value)
{
  if (value->type == TYPE_STRING)
    free (value->v_string);

  free (value);
}

static void
value_print_debug (struct value *value)
{
  if (value->type == TYPE_INTEGER)
    printf ("%i\n", value->v_integer);
  else if (value->type == TYPE_FLOAT)
    printf ("%f\n", value->v_float);
  else if (value->type == TYPE_STRING)
    printf ("%s\n", value->v_string);
  else if (value->type == TYPE_VOID)
    printf ("(void)\n");
}

char *
read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading the file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}

struct value *
evaluate (struct ast *node)
{
  // ast_print_debug (node, 0);

  if (node->type == AST_PROGRAM)
    {
      struct ast *current = node->child;
      while (current)
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
  else if (node->type == AST_RETURN)
    return evaluate (node->child);
  else if (node->type == AST_INTEGER)
    {
      struct value *value = value_create (TYPE_INTEGER);
      value->v_integer = atoi (node->token.value);
      return value; 
    }
  else if (node->type == AST_FLOAT)
    {
      struct value *value = value_create (TYPE_FLOAT);
      value->v_float = atof (node->token.value);
      return value; 
    }
  else if (node->type == AST_STRING)
    {
      struct value *value = value_create (TYPE_STRING);
      value->v_string = strdup (node->token.value);
      return value; 
    }

  return value_create (TYPE_VOID);
}

int
main (int argc, char *argv[])
{
  struct lexer *lexer;
  struct parser *parser;

  char *source = read_file ("tests/syntax.txt");

  lexer = lexer_create (source);
  parser = parser_create (lexer);

  struct ast *ast = parser_parse (parser);

  if (argc >= 2 && strcmp (argv[1], "-d") == 0)
    ast_print_debug (ast, 0);

  struct value *value = evaluate (ast);
  value_print_debug (value);
  value_destroy (value);

  ast_destroy (ast);

  parser_destroy (parser);
  lexer_destroy (lexer);

  free (source);

  return 0;
}

