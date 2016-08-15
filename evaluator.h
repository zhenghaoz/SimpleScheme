#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "variable.h"

EVAL_NAMESPACE_BEGIN

Variable eval(const Variable &exp, Environment &env);
Variable apply(const Variable &proc, const Variable &vals, Environment &env);

EVAL_NAMESPACE_END

#endif