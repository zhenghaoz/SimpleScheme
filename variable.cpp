#include <sstream>
#include "variable.h"
#include "exception.h"

EVAL_NAMESPACE_BEGIN

/* garbage collector */

default_random_engine GarbageCollector::engine;
uniform_int_distribution<int> GarbageCollector::distribution;
vector<Variable> GarbageCollector::_watchList = vector<Variable>();
Environment GarbageCollector::_globalEnv;
int GarbageCollector::_threshold = 0;

void GarbageCollector::setGlobalEnvironment(const Environment& env)
{
	_globalEnv = env;
}

void GarbageCollector::collect()
{
	int tag = RANDOM_COLOR;
	_globalEnv.setTag(tag);
	vector<Variable> nWatchList;
	for (Variable& var : _watchList)
		if (*var._refCount > 1 && *var._tag != tag)
			var.finalize();
		else if (*var._refCount > 1)
			nWatchList.push_back(var);
	std::swap(_watchList, nWatchList);
}

/* primitive procedure */

ostream &operator<<(ostream &out, const PrimitiveProcdeure &prim)
{
	if (*prim._namePtr == "")
		out << "#<procedure>";
	else
		out << "#<procedure:" + *prim._namePtr + ">";
	return out;
}

/* compound procedure */

ostream &operator<<(ostream &out, const CompoundProcedure &comp)
{
	if (*comp._namePtr == "")
		out << "#<procedure>";
	else
		out << "#<procedure:" + *comp._namePtr + ">";
	return out;
}

/* variable */

// static member variable
#ifdef EVAL_DEBUG
int Variable::_valueCreated = 0;
int Variable::_valueDestroyed = 0;
#endif
const char* Variable::_typeNames[] = { "void", "null", "number", "symbol", "pair", "procedure", "procedure" };

// util functions
std::ostream& operator<<(std::ostream& out, const Variable& var)
{
	switch (var._type) {
		case Variable::NUMBER:
			out << *var._numPtr;
			break;
		case Variable::SYMBOL:
		case Variable::STRING:
			out << *var._strPtr;
			break;
		case Variable::PAIR:
		case Variable::NIL:
			out << '(';
			Variable::printList(out, var);
			out << ')';
			break;
		case Variable::PRIM:
			out << *var._primPtr;
			break;
		case Variable::COMP:
			out << *var._compPtr;
	}
	return out;
}

void Variable::swap(Variable &a, Variable &b)
{
	std::swap(a._type, b._type);
	std::swap(a._refCount, b._refCount);
	std::swap(a._voidPtr, b._voidPtr);
	std::swap(a._tag, b._tag);
	std::swap(a._beWatching, b._beWatching);
}

void Variable::printList(std::ostream& out, const Variable &var)
{
	if (var._type != Variable::PAIR)
		return;
	const Variable &a = var.car();
	const Variable &b = var.cdr();
	out << a;
	if (b._type == PAIR) {
		out << ' ';
		printList(out, b);
	} else if (b._type == NIL) {
		return;
	} else {
		out << " . " << b;
	}
}

// interface

bool Variable::isEqual(const Variable &b) const
{
	if (_type != b._type)
		return false;
	switch (_type) {
		case Variable::NUMBER:
			return *_numPtr == *b._numPtr;
		case Variable::SYMBOL:
		case Variable::STRING:
			return *_strPtr == *b._strPtr;
		case Variable::PAIR:
			return car().isEqual(b.car()) && cdr().isEqual(b.cdr());
		case Variable::NIL:
			return _type == b._type;
		default:
			throw SchemeException(string("variable: ") + toString() + "is uncomparable.");
	}	
}

Variable::~Variable()
{
	(*_refCount)--;
	if (*_refCount == 0) {
		delete _refCount;
		switch (_type) {
			case Variable::NUMBER:
				delete _numPtr;
				VAL_DESTROYED;
				break;
			case Variable::SYMBOL:
			case Variable::STRING:
				delete _strPtr;
				VAL_DESTROYED;
				break;
			case Variable::PAIR:
				delete _pairPtr;
				delete _tag;
				delete _beWatching;
				VAL_DESTROYED;
				break;
			case Variable::PRIM:
				delete _primPtr;
				VAL_DESTROYED;
				break;
			case Variable::COMP:	/* circular reference problem */
				delete _compPtr;
				delete _tag;
				delete _beWatching;
				VAL_DESTROYED;
		}
	} else if ((_type == PAIR || _type == COMP) && *_beWatching == false) {
		*_beWatching = true;
		GarbageCollector::addVar(*this);
	}
}

// garbage collection

void Variable::setTag(int tag)
{
	// only PAIR and COMP need tag
	if (_type != PAIR && _type != COMP)
		return;
	// prevent set tag again
	if (*_tag == tag)
		return;
	// set tag
	*_tag = tag;
	if (_type == PAIR) {	// PAIR
		car().setTag(tag);
		cdr().setTag(tag);
	} else {				// COMP
		_compPtr->getSequence().setTag(tag);
		_compPtr->getArguments().setTag(tag);
		_compPtr->getEnvironmrnt().setTag(tag);
	}
}

void Variable::finalize()
{
	if (_type == PAIR) {
		setCar(VAR_NULL);
		setCdr(VAR_NULL);
	} else if (_type == COMP) {
		_compPtr->finalize();
	}
}

/* environment */

void Environment::addVars(const Variable &vars, const Variable &vals)
{
	for (Variable varIt = vars, valIt = vals; !varIt.isNull() && !valIt.isNull(); varIt = varIt.cdr(), valIt = valIt.cdr()) {
		Variable var = varIt.car();
		Variable val = valIt.car();
		(*_envPtr)[string(var)] = val;
	}
}

Environment::Environment(const Variable &vars, const Variable &vals): 
		_encloseEnvPtr(nullptr), _envPtr(std::make_shared<map>())
{
	addVars(vars, vals);
}

Environment::Environment(const Variable &vars, const Variable &vals, const Environment &encloseEnv): 
		_encloseEnvPtr(std::make_shared<Environment>(encloseEnv)), _envPtr(std::make_shared<map>())
{
	addVars(vars, vals);
}

Variable Environment::defineVariable(const Variable &var, const Variable &val)
{
	(*_envPtr)[string(var)] = val;
	return VAR_VOID;
}

map::iterator Environment::findVar(const Variable &var)
{
	string str = static_cast<string>(var);
	for (Environment *envIt = this; envIt; envIt = envIt->_encloseEnvPtr.get()) {
		auto it = envIt->_envPtr->find(str);
		if (it != envIt->_envPtr->cend())
			return it;
	}
	throw SchemeException(str + ": variable not found");
}

Variable Environment::assignVariable(const Variable &var, const Variable &val)
{
	auto it = findVar(var);
	it->second = val;
	return VAR_VOID;
}

Variable Environment::lookupVariable(const Variable &var)
{
	auto it = findVar(var);
	return it->second;
}

void Environment::setTag(int tag)
{
	for (auto it = _envPtr->begin(); it != _envPtr->end(); it++)
		it->second.setTag(tag);
}

EVAL_NAMESPACE_END