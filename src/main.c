#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"
#include "array.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

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

  printf ("PROGRAM RETURNED:\n");

  if (value != NULL)
    {
      value_print (value, stdout);
      printf ("\n");
    }

  value_destroy (value);

  ast_destroy (ast);

  parser_destroy (parser);
  lexer_destroy (lexer);

  free (source);

  return 0;
}

