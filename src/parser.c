#include "parser.h"
#include "common.h"
#include <stdlib.h>

typedef struct ast *(parse_function_t)(struct parser *);

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

static void
parser_push_until (struct parser *parser, struct ast *node, size_t type,
                   parse_function_t parse_function, bool initial)
{
  if (initial)
    ast_push (node, parse_function (parser));

  while (!token_type_match (parser->current.type, 2, type, TOKEN_EOF))
    ast_push (node, parse_function (parser));
}

static struct ast *parser_parse_statement (struct parser *parser);
static struct ast *parser_parse_expression (struct parser *parser);

static struct ast *parser_parse_declaration (struct parser *parser);
static struct ast *parser_parse_function (struct parser *parser);
static struct ast *parser_parse_array (struct parser *parser);
static struct ast *parser_parse_structure (struct parser *parser);

static struct ast *parser_parse_number (struct parser *parser);
static struct ast *parser_parse_string (struct parser *parser);
static struct ast *parser_parse_identifier (struct parser *parser);
static struct ast *parser_parse_symbol (struct parser *parser);

static struct ast *
parser_parse_program (struct parser *parser)
{
  struct ast *result;
  size_t line = parser->current.line;

  result = ast_create (AST_PROGRAM, line);
  parser_push_until (parser, result, TOKEN_RPAREN, parser_parse_statement,
                     false);

  return result;
}

static struct ast *
parser_parse_statement (struct parser *parser)
{
  size_t type = parser->current.type;
  size_t line = parser->current.line;

  if (token_type_match (type, 1, TOKEN_ARROW))
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
    return parser_parse_structure (parser);
  else if (token_type_match (type, 2, TOKEN_INTEGER, TOKEN_FLOAT))
    return parser_parse_number (parser);
  else if (token_type_match (type, 1, TOKEN_STRING))
    return parser_parse_string (parser);
  else if (token_type_match (type, 1, TOKEN_IDENTIFIER))
    {
      struct token peek = lexer_peek (parser->lexer);
      if (peek.value != NULL)
        free (peek.value);

      if (token_type_match (peek.type, 1, TOKEN_EQUALS))
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

  parser_advance_match (parser, TOKEN_EQUALS);

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
  size_t line = parser->current.line;

  parser_advance_match (parser, TOKEN_LPAREN);
  size_t type = parser->current.type;

  if (token_type_match (type, 1, TOKEN_LBRACKET))
    {
      struct ast *program;

      parser_advance_match (parser, TOKEN_LBRACKET);

      result = ast_create (AST_FUNCTION_DEFINITION, line);
      parser_push_until (parser, result, TOKEN_RBRACKET,
                         parser_parse_identifier, false);

      parser_advance_match (parser, TOKEN_RBRACKET);

      program = parser_parse_program (parser);
      ast_push (result, program);
    }
  else
    {
      result = ast_create (AST_FUNCTION_INVOCATION, line);
      parser_push_until (parser, result, TOKEN_RPAREN, parser_parse_expression,
                         true);
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

  result = ast_create (AST_ARRAY, line);
  parser_push_until (parser, result, TOKEN_RBRACKET, parser_parse_expression,
                     false);

  parser_advance_match (parser, TOKEN_RBRACKET);

  return result;
}

static struct ast *
parser_parse_structure (struct parser *parser)
{
  struct ast *result;
  size_t line = parser->current.line;

  parser_advance_match (parser, TOKEN_LBRACE);

  result = ast_create (AST_STRUCTURE, line);
  parser_push_until (parser, result, TOKEN_RBRACE, parser_parse_declaration,
                     false);

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

  parser_match (parser, TOKEN_STRING);

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

  parser_match (parser, TOKEN_SYMBOL);

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

