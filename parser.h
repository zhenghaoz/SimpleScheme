#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "variable.h"

EVAL_NAMESPACE_BEGIN

Variable parse(const std::string &str);

EVAL_NAMESPACE_END

#endif