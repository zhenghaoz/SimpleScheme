#include "variable.h"
#include "exception.h"

EVAL_NAMESPACE_BEGIN

/* type names */

int Variable::_valueCreated = 0;
int Variable::_valueDestroyed = 0;
const char* Variable::_typeNames[] = { "null", "number", "symbol", "pair" };

/* helper functions */

std::ostream& operator<<(std::ostream& out, const Variable& var)
{
	switch (var._type) {
		case Variable::NUMBER:
			out << *var._intPtr;
			break;
		case Variable::SYMBOL:
			out << *var._strPtr;
			break;
		case Variable::PAIR:
		case Variable::NIL:
			out << '(';
			Variable::printList(out, var);
			out << ')';
	}
	return out; 
}

void Variable::swap(Variable &a, Variable &b)
{
	std::swap(a._type, b._type);
	std::swap(a._refCount, b._refCount);
	std::swap(a._voidPtr, b._voidPtr);
}

void Variable::printList(std::ostream& out, const Variable &var)
{
	if (var._type != Variable::PAIR)
		return;
	const Variable &a = car(var);
	const Variable &b = cdr(var);
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

Variable Variable::operator&&(const Variable &b)
{
	if (_type == Variable::SYMBOL && *_strPtr == "false")
		return VAR_FALSE;
	if (b._type == Variable::SYMBOL && *b._strPtr == "false")
		return VAR_FALSE;
	return VAR_TRUE;
}

/* public member functions */

void Variable::requireType(std::string sender, Type type) const
{
	if (_type != type)
		throw SchemeException(sender + ": contract violation\n\
			\texpected: " + _typeNames[type] + "\n\
			\tgiven: " + "\n");
}

Variable::Variable(const Variable &var): _type(var._type), _refCount(var._refCount)
{
	(*_refCount)++;
	switch (var._type) {
		case Variable::NUMBER:
			_intPtr = var._intPtr;
			break;
		case Variable::SYMBOL:
			_strPtr = var._strPtr;
			break;
		case Variable::PAIR:
			_pairPtr = var._pairPtr;
	}
}

Variable& Variable::operator=(Variable var)
{
	swap(*this, var);
}

Variable::~Variable()
{
	(*_refCount)--;
	if (*_refCount == 0) {
		delete _refCount;
		switch (_type) {
			case Variable::NUMBER:
				delete _intPtr;
				VAL_DESTROYED;
				break;
			case Variable::SYMBOL:
				delete _strPtr;
				VAL_DESTROYED;
				break;
			case Variable::PAIR:
				delete _pairPtr;
				VAL_DESTROYED;
		}
	}
}

/* variable operator */

Variable isNull(const Variable &var)
{
	return var._type == Variable::NIL ? VAR_TRUE : VAR_FALSE;
}

Variable isNumber(const Variable &var)
{
	return var._type == Variable::NUMBER ? VAR_TRUE : VAR_FALSE;
}

Variable isSymbol(const Variable &var)
{
	return var._type == Variable::SYMBOL ? VAR_TRUE : VAR_FALSE;
}

Variable isPair(const Variable &var)
{
	return var._type == Variable::PAIR ? VAR_TRUE : VAR_FALSE;
}

Variable isEqual(const Variable &a, const Variable &b)
{
	if (a._type != b._type)
		return VAR_FALSE;
	switch (a._type) {
		case Variable::NUMBER:
			return a == b;
		case Variable::SYMBOL:
			return *a._strPtr == *b._strPtr;
		case Variable::PAIR:
			return isEqual(car(a), car(b)) && isEqual(cdr(a), cdr(b));
	}	
}

Variable operator+(const Variable &a, const Variable &b)
{
	a.requireType("+", Variable::NUMBER);
	b.requireType("+", Variable::NUMBER);
	return Variable(*a._intPtr + *b._intPtr);
}

Variable operator-(const Variable &a, const Variable &b)
{
	a.requireType("-", Variable::NUMBER);
	b.requireType("-", Variable::NUMBER);
	return Variable(*a._intPtr - *b._intPtr);
}

Variable operator*(const Variable &a, const Variable &b)
{
	a.requireType("*", Variable::NUMBER);
	b.requireType("*", Variable::NUMBER);
	return Variable(*a._intPtr * *b._intPtr);
}

Variable operator/(const Variable &a, const Variable &b)
{
	a.requireType("/", Variable::NUMBER);
	b.requireType("/", Variable::NUMBER);
	if (*b._intPtr == 0)
		throw SchemeException("/: divide by zero");
	return Variable(*a._intPtr / *b._intPtr);
}

Variable operator<(const Variable &a, const Variable &b)
{
	a.requireType("<", Variable::NUMBER);
	b.requireType("<", Variable::NUMBER);
	return *a._intPtr < *b._intPtr ? VAR_TRUE : VAR_FALSE;
}

Variable operator>(const Variable &a, const Variable &b)
{
	a.requireType(">", Variable::NUMBER);
	b.requireType(">", Variable::NUMBER);
	return *a._intPtr > *b._intPtr ? VAR_TRUE : VAR_FALSE;
}

Variable operator==(const Variable &a, const Variable &b)
{
	a.requireType("=", Variable::NUMBER);
	b.requireType("=", Variable::NUMBER);
	return *a._intPtr == *b._intPtr ? VAR_TRUE : VAR_FALSE;
}

Variable cons(const Variable &a, const Variable &b)
{
	return Variable(a, b);
}

Variable car(const Variable &var)
{
	var.requireType("car", Variable::PAIR);
	return var._pairPtr->first;
}

Variable cdr(const Variable &var)
{
	var.requireType("cdr", Variable::PAIR);
	return var._pairPtr->second;
}

Variable setCar(Variable pair, const Variable &var)
{
	pair.requireType("set-car!", Variable::PAIR);
	pair._pairPtr->first = var;
	return pair;
}

Variable setCdr(Variable pair, const Variable &var)
{
	pair.requireType("set-cdr!", Variable::PAIR);
	pair._pairPtr->second = var;
	return pair;
}

EVAL_NAMESPACE_END