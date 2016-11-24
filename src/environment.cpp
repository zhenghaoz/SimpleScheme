// 
// Scheme Environment
// 
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include "environment.hpp"
#include "variable.hpp"

// Constructor for top-environment
Environment::Environment(const Variable &vars, const Variable &vals): 
		encloseEnvPtr(nullptr), framePtr(std::make_shared<frame>())
{
	addVars(vars, vals);
}

// Constructor for sub-environment
Environment::Environment(const Variable &vars, const Variable &vals, const Environment &encloseEnv): 
		encloseEnvPtr(std::make_shared<Environment>(encloseEnv)), framePtr(std::make_shared<frame>())
{
	addVars(vars, vals);
}

// Add variables into environment
void Environment::addVars(const Variable &vars, const Variable &vals)
{
	for (Variable varIt = vars, valIt = vals; !varIt.isNull() && !valIt.isNull(); varIt = varIt.cdr(), valIt = valIt.cdr()) {
		Variable var = varIt.car();
		Variable val = valIt.car();
		var.requireType("define variable", Variable::TYPE_SYMBOL);
		(*framePtr)[var.toString()] = val;
	}
}

// Find variable in environment
Environment::frame::iterator Environment::findVar(const string &var)
{
	// Find variable from inner env to outer env
	for (Environment *envIt = this; envIt; envIt = envIt->encloseEnvPtr.get()) {
		auto it = envIt->framePtr->find(var);
		if (it != envIt->framePtr->cend())
			return it;
	}
	// Variable not found
	throw Exception(var + ": variable not found");
}

// Define variable
Variable Environment::defineVariable(const Variable& var, const Variable& val)
{
	var.requireType("define", Variable::TYPE_SYMBOL);
	(*framePtr)[var.toString()] = val;
	return VAR_VOID;
}

// Assign variable
Variable Environment::assignVariable(const Variable &var, const Variable &val)
{
	var.requireType("set!", Variable::TYPE_SYMBOL);
	auto it = findVar(var.toString());
	it->second = val;
	return VAR_VOID;
}

// Lookup variable
Variable Environment::lookupVariable(const Variable &var)
{
	var.requireType("lookup variable", Variable::TYPE_SYMBOL);
	auto it = findVar(var.toString());
	return it->second;
}