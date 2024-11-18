#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "value.h"

struct value *evaluate (struct ast *node);

#endif // EVALUATOR_H

