#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

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
main (void)
{
  struct lexer *lexer;
  struct parser *parser;

  char *source = read_file ("tests/syntax.txt");

  lexer = lexer_create (source);
  parser = parser_create (lexer);

  struct ast *ast = parser_parse (parser);
  ast_print_debug (ast, 0);

  ast_destroy (ast);

  parser_destroy (parser);
  lexer_destroy (lexer);

  free (source);

  return 0;
}

