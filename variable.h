#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <sstream>
#include "evaldef.h"

EVAL_NAMESPACE_BEGIN

/* declare */

class Variable;
class Environment;

/* alias */

template <typename T> using shared_ptr = std::shared_ptr<T>;
using number = int;
using pair = std::pair<Variable, Variable>;
using ostream = std::ostream;
using string = std::string;
using map = std::map<string, Variable>;
using procedure = std::function<Variable(const Variable&)>;

/* dynamic object tracer */

#ifdef EVAL_DEBUG

#define VAL_CREATE	do { _valueCreated++; } while (0)
#define VAL_DESTROYED	do { _valueDestroyed++; } while (0)
#define VAL_DEBUG_STATE	do { std::clog << "[value] created: " << Variable::_valueCreated << " destroyed: " << Variable::_valueDestroyed << std::endl; } while(0)

#else

#define VAL_CREATE	do {} while (0)
#define VAL_DESTROYED	do {} while (0)

#endif

/* primitive procedure */

class PrimitiveProcdeure
{
	friend ostream &operator<<(ostream &out, const PrimitiveProcdeure &prim);
	shared_ptr<string> _namePtr;
	shared_ptr<procedure> _proc;
public:
	PrimitiveProcdeure(const string &name, const procedure &proc): 
		_namePtr(std::make_shared<string>(name)), 
		_proc(std::make_shared<procedure>(proc)) {}
	inline Variable operator() (const Variable &args);
	string getName() const { return *_namePtr; }
};

/* compound procedure */

class CompoundProcedure
{
	friend ostream &operator<<(ostream &out, const CompoundProcedure &comp);
	shared_ptr<string> _namePtr;
	shared_ptr<Environment> _envPtr;
	shared_ptr<Variable> _seqPtr, _argsPtr;
public:
	inline CompoundProcedure(const Variable &name, const Variable &args, const Variable &seq, const Environment &env);
	inline CompoundProcedure(const Variable &args, const Variable &seq, const Environment &env);
	inline Variable getSequence() const;
	inline Variable getArguments() const;
	inline Environment getEnvironmrnt() const;
};

/* variable */

class Variable
{
public:
	// public member 
	#ifdef EVAL_DEBUG
	static int _valueCreated;
	static int _valueDestroyed;
	#endif
	static const char* _typeNames[];
	enum Type { VOID, NIL, NUMBER, SYMBOL, STRING, PAIR, PRIM, COMP };
	// output
	friend std::ostream& operator<<(std::ostream& out, const Variable& var);
private:
	// private member variable
	union {
		void *_voidPtr;
		number *_numPtr;
		string *_strPtr;
		pair *_pairPtr;
		PrimitiveProcdeure *_primPtr;
		CompoundProcedure *_compPtr;
	};
	int *_refCount;
	Type _type;
private:
	// util function
	static void swap(Variable &a, Variable &b);
	static void printList(std::ostream& out, const Variable &var);
public:
	// constructors
	Variable(): _type(NIL), _refCount(new int(1)) {}
	Variable(number num, Type type = NUMBER): 
		_numPtr(new number(num)), _type(type), _refCount(new int(1)) { VAL_CREATE; }
	Variable(const string &str, Type type = SYMBOL): 
		_strPtr(new string(str)), _type(type), _refCount(new int(1)) { VAL_CREATE; }
	Variable(const PrimitiveProcdeure &prim): 
		_primPtr(new PrimitiveProcdeure(prim)), _type(PRIM), _refCount(new int(1)) {}
	Variable(const Variable &name, const Variable &args, const Variable &seq, const Environment &env):
		_compPtr(new CompoundProcedure(name, args, seq, env)), _type(COMP), _refCount(new int(1)) {}
	Variable(const Variable &args, const Variable &seq, const Environment &env):
		Variable(Variable(""), args, seq, env) {}
	Variable(const Variable &a, const Variable &b): 
		_pairPtr(new pair(a, b)), _type(PAIR), _refCount(new int(1)) { VAL_CREATE; }
	Variable(const Variable &var);
	Variable& operator=(Variable var);
	~Variable();
	// interface
	void requireType(const string &caller, Type type) const;
	string toString() const { std::stringstream ss; ss << (*this); return ss.str(); }
	bool isNull() const { return _type == NIL; }
	bool isNumber() const { return _type == NUMBER; }
	bool isVoid() const { return _type == VOID; }
	bool isPair() const { return _type == PAIR; }
	bool isSymbol() const { return _type == SYMBOL; }
	bool isString() const { return _type == STRING; }
	bool isPrim() const { return _type == PRIM; }
	bool isComp() const { return _type == COMP; }
	bool isEqual(const Variable &var) const;
	bool isEqual(const string &str) const { return isSymbol() && *_strPtr == str; }
	Variable car() const { requireType("car", PAIR); return _pairPtr->first; }
	Variable cdr() const { requireType("cdr", PAIR); return _pairPtr->second; }
	inline Variable setCar(const Variable &var);
	inline Variable setCdr(const Variable &var);
	explicit operator bool() { return _type != SYMBOL || *_strPtr != "false"; }
	explicit operator number() const;
	explicit operator string() const;
	explicit operator CompoundProcedure() const;
	Variable operator() (const Variable &var);
};

/* constants */

const Variable VAR_TRUE = Variable("true", Variable::SYMBOL);
const Variable VAR_FALSE = Variable("false", Variable::SYMBOL);
const Variable VAR_NULL = Variable();
const Variable VAR_VOID = Variable(0, Variable::VOID);

/* environment */

class Environment
{
	shared_ptr<Environment> _encloseEnvPtr;
	shared_ptr<map> _envPtr;
	void addVars(const Variable &vars, const Variable &vals);
	map::iterator findVar(const Variable &var);
public:
	Environment(const Variable &vars = VAR_NULL, const Variable &vals = VAR_NULL);
	Environment(const Variable &vars, const Variable &vals, const Environment &encloseEnv);
	Variable assignVariable(const Variable &var, const Variable &val);
	Variable defineVariable(const Variable &var, const Variable &val);
	Variable lookupVariable(const Variable &var);
};

/* inline functions */

Variable PrimitiveProcdeure::operator() (const Variable &args) { return (*_proc)(args); }

CompoundProcedure::CompoundProcedure(const Variable &name, const Variable &args, const Variable &seq, const Environment &env):
	_namePtr(std::make_shared<string>(string(name))),
	_argsPtr(std::make_shared<Variable>(args)), 
	_seqPtr(std::make_shared<Variable>(seq)), 
	_envPtr(std::make_shared<Environment>(env)) {}
CompoundProcedure::CompoundProcedure(const Variable &args, const Variable &seq, const Environment &env):
	CompoundProcedure(Variable(""), args, seq, env) {}
Variable CompoundProcedure::getSequence() const { return *_seqPtr; }
Variable CompoundProcedure::getArguments() const { return *_argsPtr; }
Environment CompoundProcedure::getEnvironmrnt() const { return *_envPtr; }

Variable Variable::setCar(const Variable &var) 
{ 
	requireType("set-car!", PAIR); 
	_pairPtr->first = var; 
	return VAR_VOID; 
}

Variable Variable::setCdr(const Variable &var)
{ 
	requireType("set-cdr!", PAIR); 
	_pairPtr->second = var; 
	return VAR_VOID; 
}

EVAL_NAMESPACE_END

#endif