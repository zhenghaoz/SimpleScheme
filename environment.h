#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <string>
#include <memory>
#include "variable.h"
#include "evaldef.h"
#include "variable.h"

EVAL_NAMESPACE_BEGIN

/* debug code */

#ifdef EVAL_DEBUG

#define ENV_CREATE	do { _envCreated++; } while (0)
#define ENV_DESTROYED	do { _envDestroyed++; } while (0)
#define ENV_DEBUG_STATE	do { std::clog << "[env] created: " << Environment::_envCreated << " destroyed: " << Environment::_envDestroyed << std::endl; } while(0)

#else

#define ENV_CREATE	do {} while (0)
#define ENV_DESTROYED	do {} while (0)

#endif

class Environment
{
public:
	/* debug code */
	#ifdef EVAL_DEBUG
	static int _envCreated;
	static int _envDestroyed;
	#endif
private:
	using string = std::string;
	using map = std::map<string, Variable>;
	using ptr = std::shared_ptr<Environment>;
	Environment *_encloseEnv;
	int *_refCount;
	map *_env;
	void addVars(const Variable &vars, const Variable &vals);
	map::iterator findVar(const std::string &str);
	static void swap(Environment &a, Environment &b);
public:
	Environment(const Variable &vars, const Variable &vals);
	Environment(const Variable &vars, const Variable &vals, const Environment &encloseEnv);
	Environment(const Environment &env);
	Environment& operator=(Environment env);
	~Environment();
	void assignVariable(const std::string &str, const Variable &var);
	void defineVariable(const std::string &str, const Variable &var);
	Variable lookupVariable(const std::string &str);
};

EVAL_NAMESPACE_END

#endif