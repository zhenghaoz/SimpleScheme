#include "environment.h"
#include "variable.h"
#include "exception.h"

EVAL_NAMESPACE_BEGIN

/* static member variable */

#ifdef EVAL_DEBUG
int Environment::_envCreated = 0;
int Environment::_envDestroyed = 0;
#endif

void Environment::swap(Environment &a, Environment &b)
{
	std::swap(a._env, b._env);
	std::swap(a._refCount, b._refCount);
	std::swap(a._encloseEnv, b._encloseEnv);
}

void Environment::addVars(const Variable &vars, const Variable &vals)
{
	for (Variable varIt = vars, valIt = vals; !(isNull(varIt) && isNull(valIt)); varIt = cdr(varIt), valIt = cdr(valIt)) {
		Variable var = car(varIt);
		Variable val = car(valIt);
		(*_env)[std::string(var)] = val;
	}
}

Environment::Environment(const Variable &vars, const Variable &vals): _refCount(new int(1)), _encloseEnv(nullptr), _env(new map)
{
	ENV_CREATE;
	addVars(vars, vals);
}

Environment::Environment(const Variable &vars, const Variable &vals, const Environment &encloseEnv): _refCount(new int(1)), _encloseEnv(new Environment(encloseEnv)), _env(new map)
{
	ENV_CREATE;
	addVars(vars, vals);
}

Environment& Environment::operator=(Environment env)
{
	swap(*this, env);
}

Environment::Environment(const Environment& env): _refCount(env._refCount), _encloseEnv(env._encloseEnv), _env(env._env)
{
	(*_refCount)++;
}

Environment::~Environment()
{
	(*_refCount)--;
	if (*_refCount == 0) {
		delete _refCount;
		delete _env;
		delete _encloseEnv;
		ENV_DESTROYED;
/*		ENV_DEBUG_STATE;*/
	}
}

void Environment::defineVariable(const std::string &str, const Variable &var)
{
	(*_env)[str] = var;
}

Environment::map::iterator Environment::findVar(const std::string &str)
{
	for (Environment *envIt = this; envIt; envIt = envIt->_encloseEnv) {
		auto it = envIt->_env->find(str);
		if (it != envIt->_env->cend())
			return it;
	}
	throw SchemeException(str + ": variable not found");
}

void Environment::assignVariable(const std::string &str, const Variable &var)
{
	auto it = findVar(str);
	it->second = var;
}

Variable Environment::lookupVariable(const std::string &str)
{
	auto it = findVar(str);
	return it->second;
}

EVAL_NAMESPACE_END