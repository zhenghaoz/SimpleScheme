// 
// Scheme Evaluator
// 
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#pragma once

#include "variable.hpp"

// Evaluate dispatcher
Variable eval(const Variable &exp, Environment &env);

// Apply procedure
Variable apply(const Variable &proc, const Variable &vals, Environment &env);
