#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include "variable.h"

EVAL_NAMESPACE_BEGIN

std::istream &operator>>(std::istream &in, Variable &var);

EVAL_NAMESPACE_END

#endif