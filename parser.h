#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>
#include "variable.h"

EVAL_NAMESPACE_BEGIN

using istream = std::istream;
using string = std::string;

istream &operator>>(istream &in, Variable &var);

EVAL_NAMESPACE_END

#endif