#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "variable.h"

EVAL_NAMESPACE_BEGIN

Variable eval(const Variable &exp, Environment &env);
Environment setupEnvironment();

EVAL_NAMESPACE_END

#endif