#include "common.h"
#include "parser.h"
#include <stdlib.h>

static void
parser_advance (struct parser *parser)
{
  parser->current = lexer_next (parser->lexer);
}

static void
parser_match (struct parser *parser, size_t type)
{
  if (!token_type_match (parser->current.type, 1, type))
    error (parser->current.line, "expected %s token", token_type_string (type));
}

static void
parser_advance_match (struct parser *parser, size_t type)
{
  parser_match (parser, type);
  parser_advance (parser);
}

static struct ast *parser_parse_statement (struct parser *parser);
static struct ast *parser_parse_expression (struct parser *parser);

static struct ast *parser_parse_declaration (struct parser *parser);
static struct ast *parser_parse_function (struct parser *parser);
static struct ast *parser_parse_array (struct parser *parser);
static struct ast *parser_parse_struct (struct parser *parser);

static struct ast *parser_parse_number (struct parser *parser);
static struct ast *parser_parse_string (struct parser *parser);
static struct ast *parser_parse_identifier (struct parser *parser);
static struct ast *parser_parse_symbol (struct parser *parser);

static struct ast *
parser_parse_program (struct parser *parser)
{
  struct ast *result;
  size_t type = parser->current.type;
  size_t line = parser->current.line;

  result = ast_create (AST_PROGRAM, line);

  while (!token_type_match (type, 2, TOKEN_RPAREN, TOKEN_EOF))
    {
      struct ast *statement;

      statement = parser_parse_statement (parser);
      ast_push (result, statement);

      type = parser->current.type;
    }

  return result;
}

static struct ast *
parser_parse_statement (struct parser *parser)
{
  size_t type = parser->current.type;
  size_t line = parser->current.line;

  if (token_type_match (type, 1, TOKEN_WALRUS))
    {
      struct ast *result;
      struct ast *expression;

      parser_advance (parser);
      expression = parser_parse_expression (parser);

      result = ast_create (AST_RETURN, line);
      ast_push (result, expression);

      return result;
    }

  return parser_parse_expression (parser);
}

static struct ast *
parser_parse_expression (struct parser *parser)
{
  size_t type = parser->current.type;

  if (token_type_match (type, 1, TOKEN_LPAREN))
    return parser_parse_function (parser);
  else if (token_type_match (type, 1, TOKEN_LBRACKET))
    return parser_parse_array (parser);
  else if (token_type_match (type, 1, TOKEN_LBRACE))
    return parser_parse_struct (parser);
  else if (token_type_match (type, 2, TOKEN_INTEGER, TOKEN_FLOAT))
    return parser_parse_number (parser);
  else if (token_type_match (type, 1, TOKEN_STRING))
    return parser_parse_string (parser);
  else if (token_type_match (type, 1, TOKEN_IDENTIFIER))
    {
      struct token peek = lexer_peek (parser->lexer);
      if (peek.value != NULL)
        free (peek.value);

      if (token_type_match (peek.type, 1, TOKEN_WALRUS))
        return parser_parse_declaration (parser);
      return parser_parse_identifier (parser);
    }
  else if (token_type_match (type, 1, TOKEN_SYMBOL))
    return parser_parse_symbol (parser);

  error (parser->current.line, "expected expression");
}

static struct ast *
parser_parse_declaration (struct parser *parser)
{
  struct ast *result;
  struct ast *identifier;
  struct ast *expression;
  size_t line = parser->current.line;

  identifier = parser_parse_identifier (parser);
  parser_advance_match (parser, TOKEN_WALRUS);

  expression = parser_parse_expression (parser);

  result = ast_create (AST_VARIABLE_DECLARATION, line);
  ast_push (result, identifier);
  ast_push (result, expression);

  return result;
}

static struct ast *
parser_parse_function (struct parser *parser)
{
  struct ast *result;

  parser_advance_match (parser, TOKEN_LPAREN);

  size_t line = parser->current.line;
  size_t type = parser->current.type;

  if (token_type_match (type, 1, TOKEN_LBRACKET))
    {
      struct ast *program;

      parser_advance (parser);
      type = parser->current.type;

      result = ast_create (AST_FUNCTION_DEFINITION, line);

      while (!token_type_match (type, 2, TOKEN_RBRACKET, TOKEN_EOF))
        {
          struct ast *identifier;

          identifier = parser_parse_identifier (parser);
          ast_push (result, identifier);

          type = parser->current.type;
        }

      parser_advance_match (parser, TOKEN_RBRACKET);

      program = parser_parse_program (parser);
      ast_push (result, program);
    }
  else
    {
      result = ast_create (AST_FUNCTION_INVOCATION, line);

      do
        {
          struct ast *expression;

          expression = parser_parse_expression (parser);
          ast_push (result, expression);

          type = parser->current.type;
        }
      while (!token_type_match (type, 2, TOKEN_RPAREN, TOKEN_EOF));
    }

  parser_advance_match (parser, TOKEN_RPAREN);

  return result;
}

static struct ast *
parser_parse_array (struct parser *parser)
{
  struct ast *result;
  size_t line = parser->current.line;

  parser_advance_match (parser, TOKEN_LBRACKET);
  size_t type = parser->current.type;

  result = ast_create (AST_ARRAY, line);

  while (!token_type_match (type, 2, TOKEN_RBRACKET, TOKEN_EOF))
    {
      struct ast *expression;

      expression = parser_parse_expression (parser);
      ast_push (result, expression);

      type = parser->current.type;
    }

  parser_advance_match (parser, TOKEN_RBRACKET);

  return result;
}

static struct ast *
parser_parse_struct (struct parser *parser)
{
  struct ast *result;
  size_t line = parser->current.line;

  parser_advance_match (parser, TOKEN_LBRACE);
  size_t type = parser->current.type;

  result = ast_create (AST_STRUCT, line);

  while (!token_type_match (type, 2, TOKEN_RBRACE, TOKEN_EOF))
    {
      struct ast *declaration;

      declaration = parser_parse_declaration (parser);
      ast_push (result, declaration);

      type = parser->current.type;
    }

  parser_advance_match (parser, TOKEN_RBRACE);

  return result;
}

static struct ast *
parser_parse_number (struct parser *parser)
{
  struct ast *result;
  size_t type = parser->current.type;
  size_t line = parser->current.line;

  result = ast_create (type == TOKEN_INTEGER ? AST_INTEGER : AST_FLOAT, line);
  result->token = parser->current;

  parser_advance (parser);

  return result;
}

static struct ast *
parser_parse_string (struct parser *parser)
{
  struct ast *result;
  size_t line = parser->current.line;

  result = ast_create (AST_STRING, line);
  result->token = parser->current;

  parser_advance (parser);

  return result;
}

static struct ast *
parser_parse_identifier (struct parser *parser)
{
  struct ast *identifier;
  size_t line = parser->current.line;

  parser_match (parser, TOKEN_IDENTIFIER);

  identifier = ast_create (AST_IDENTIFIER, line);
  identifier->token = parser->current;

  parser_advance (parser);

  return identifier;
}

static struct ast *
parser_parse_symbol (struct parser *parser)
{
  struct ast *result;
  size_t line = parser->current.line;

  result = ast_create (AST_SYMBOL, line);
  result->token = parser->current;

  parser_advance (parser);

  return result;
}

struct parser *
parser_create (struct lexer *lexer)
{
  struct parser *parser;

  parser = calloc (1, sizeof (struct parser));
  parser->lexer = lexer;

  return parser;
}

void
parser_destroy (struct parser *parser)
{
  free (parser);
}

struct ast *
parser_parse (struct parser *parser)
{
  parser_advance (parser);

  struct ast *result;

  result = parser_parse_program (parser);
  parser_advance_match (parser, TOKEN_EOF);

  return result;
}

