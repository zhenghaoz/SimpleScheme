#include "variable.hpp"
#include "exception.hpp"

EVAL_NAMESPACE_BEGIN

/* garbage collector */

default_random_engine GarbageCollector::engine;
uniform_int_distribution<int> GarbageCollector::distribution;
vector<Variable> GarbageCollector::_watchList = vector<Variable>();
Environment GarbageCollector::_globalEnv;

void GarbageCollector::setGlobalEnvironment(const Environment& env) { _globalEnv = env; }

void GarbageCollector::addVar(const Variable& var) { _watchList.push_back(var); }

void GarbageCollector::collect()
{
	int tag = distribution(engine);
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

PrimitiveProcdeure::PrimitiveProcdeure(const string &name, const procedure &proc): 
	_namePtr(std::make_shared<string>(name)), 
	_proc(std::make_shared<procedure>(proc)) {}

Variable PrimitiveProcdeure::operator() (const Variable &args, Environment &env) const { return (*_proc)(args, env); }

string PrimitiveProcdeure::getName() const { return *_namePtr; }

ostream &operator<<(ostream &out, const PrimitiveProcdeure &prim)
{
	if (*prim._namePtr == "")
		out << "#<procedure>";
	else
		out << "#<procedure:" + *prim._namePtr + ">";
	return out;
}

/* compound procedure */

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

void CompoundProcedure::finalize() 
{ 
	_envPtr.reset(); 
	_namePtr.reset(); 
	_seqPtr.reset();
	_argsPtr.reset();
}

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

#ifdef EVAL_TRACE_MEMORY
int Variable::_valueCreated = 0;
int Variable::_valueDestroyed = 0;
#endif

const vector<string> Variable::_typeNames = { "void", "null", "number", "string", "symbol", "pair", "procedure", "procedure" };

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

void Variable::printList(std::ostream& out, const Variable &var)
{
	if (var._type != Variable::PAIR)
		return;
	const Variable &a = var.car();
	const Variable &b = var.cdr();
	out << a;
	if (b._type == PAIR) {		// print item
		out << ' ';
		printList(out, b);
	} else if (b._type == NIL) {// reach the end of list
		return;
	} else {					// not a list
		out << " . " << b;
	}
}

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

void Variable::swap(Variable &a, Variable &b)
{
	std::swap(a._voidPtr, b._voidPtr);
	std::swap(a._refCount, b._refCount);
	std::swap(a._type, b._type);
	std::swap(a._tag, b._tag);
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
				VAL_DESTROYED;
				break;
			case Variable::PRIM:
				delete _primPtr;
				VAL_DESTROYED;
				break;
			case Variable::COMP:
				delete _compPtr;
				delete _tag;
				VAL_DESTROYED;
		}
	}
}

void Variable::requireType(const std::string &caller, Type type) const
{
	if (_type != type) 
		throw SchemeException(caller + ": contract violation\n\texpected: " + _typeNames[type] + "\n\tgiven: " + toString());
}

Variable::operator bool() const 
{ 
	return _type != SYMBOL || *_strPtr != "false"; 
}

Variable::operator number() const
{
	if (!isNumber())
		throw SchemeException(string("variable: can't convert ") + toString() + " to number");
	return *_numPtr;
}

Variable::operator string() const
{
	if (!isSymbol() && !isString())
		throw SchemeException(string("variable: can't convert ") + toString() + " to string");
	return *_strPtr;
}

Variable::operator CompoundProcedure() const
{
	if (!isComp())
		throw SchemeException(string("variable: can't convert ") + toString() + " to compound procedure");
	return *_compPtr;
}

Variable Variable::operator() (const Variable &var, Environment &env) const
{
	if (!isPrim())
		throw SchemeException(string("variable: ") + toString() + " isn't a primitive procedure.");
	return (*_primPtr)(var, env);
}

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
		_encloseEnvPtr(nullptr), _envPtr(std::make_shared<map<string, Variable>>())
{
	addVars(vars, vals);
}

Environment::Environment(const Variable &vars, const Variable &vals, const Environment &encloseEnv): 
		_encloseEnvPtr(std::make_shared<Environment>(encloseEnv)), _envPtr(std::make_shared<map<string, Variable>>())
{
	addVars(vars, vals);
}

Variable Environment::defineVariable(const Variable &var, const Variable &val)
{
	(*_envPtr)[string(var)] = val;
	return VAR_VOID;
}

map<string, Variable>::iterator Environment::findVar(const Variable &var)
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

void Environment::finalize()
{
	for (auto it = _envPtr->begin(); it != _envPtr->end(); it++)
		it->second = VAR_NULL;
}

EVAL_NAMESPACE_END