//
// Variable for Scheme
//
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include "variable.hpp"

// Constant values
const Variable VAR_NULL		= Variable();
const Variable VAR_VOID		= Variable();
const Variable VAR_TRUE		= Variable();
const Variable VAR_FALSE	= Variable();

// Type alias
using ostream = std::ostream;
using string = std::string;

// Standard output
ostream& operator<<(ostream& out, const Variable& var)
{
	// List
	if (var == VAR_NULL || var.type == Variable::PAIR) {
		out << '(';
		Variable it = var;
		bool empty = true;
		while (it.isPair()) {
			if (empty) 
				empty = false;
			else
				out << ' ';
			out << it.car();
			it = it.cdr();
		}
		if (it != VAR_NULL)
			out << " . " << it;
		out << ')';
		return out;
	}
	// Single item
	switch (var.type) {
		case Variable::SPEC:
			if (var == VAR_TRUE)
				out << "#t";
			else if (var == VAR_FALSE)
				out << "#f";
			break;
		case Variable::RATIONAL:
			out << *(var.rationalPtr);
			break;
		case Variable::DOUBLE:
			out << *(var.doublePtr);
			break;
		case Variable::STRING:
			out << '"' << *(var.stringPtr) << '"';
			break;
		case Variable::SYMBOL:
			out << *(var.stringPtr);
			break;
		case Variable::PRIM:
		case Variable::COMP:
			out << "#<procedure:" + var.getProcedureName() + ">";
			break;
		default:
			;
	}
	return out;
}

// Require type

void Variable::requireType(const string &caller, int type) const
{
	if (this->type & type)
		return;
	throw Exception(caller + ": contract violation\n" 
		+ "\texpected: " + getTypeName(type) + "\n"
		+ "\tgiven: " + this->toString());
}

// Get type name

string Variable::getTypeName(int type)
{
	switch (type) {
		case DOUBLE:
			return "double";
		case RATIONAL:
			return "rational";
		case RATIONAL | DOUBLE:
			return "number";
		case PAIR:
			return "pair";
		case STRING:
			return "string";
		case SYMBOL:
			return "symbol";
		case PRIM:
			return "primitive";
		case COMP:
			return "compound";
		case COMP | PRIM:
			return "procedure";
		default:
			return "<unkown>";
	}
}

// Convert operations

string Variable::toString() const
{
	ostringstream out;
	out << (*this);
	return out.str();
}

double Variable::toDouble() const
{
	requireType("convert to double", DOUBLE | RATIONAL);
	if (type == RATIONAL)
		return static_cast<double>(*rationalPtr);
	return *doublePtr;
}

// Check operations

bool Variable::isNull() const
{
	return refCount == VAR_NULL.refCount;
}

bool Variable::isVoid() const
{
	return refCount == VAR_VOID.refCount;
}

bool Variable::isPair() const
{
	return type == PAIR;
}

bool Variable::isNumber() const
{
	return type & (RATIONAL | DOUBLE);
}

bool Variable::isSymbol() const
{
	return type == SYMBOL;
}

bool Variable::isString() const
{
	return type == STRING;
}

bool Variable::isProcedure() const
{
	return type & (COMP | PRIM);
}

// Arithmetic operations

Variable operator+(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("+", Variable::DOUBLE | Variable::RATIONAL);
	rhs.requireType("+", Variable::DOUBLE | Variable::RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::DOUBLE:
			return Variable(*lhs.doublePtr + *rhs.doublePtr);
		case Variable::RATIONAL:
			return Variable(*lhs.rationalPtr + *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() + rhs.toDouble());
	}
}

Variable operator-(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("-", Variable::DOUBLE | Variable::RATIONAL);
	rhs.requireType("-", Variable::DOUBLE | Variable::RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::DOUBLE:
			return Variable(*lhs.doublePtr - *rhs.doublePtr);
		case Variable::RATIONAL:
			return Variable(*lhs.rationalPtr - *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() - rhs.toDouble());
	}
}

Variable operator*(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("*", Variable::DOUBLE | Variable::RATIONAL);
	rhs.requireType("*", Variable::DOUBLE | Variable::RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::DOUBLE:
			return Variable(*lhs.doublePtr * *rhs.doublePtr);
		case Variable::RATIONAL:
			return Variable(*lhs.rationalPtr * *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() * rhs.toDouble());
	}
}

Variable operator/(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("/", Variable::DOUBLE | Variable::RATIONAL);
	rhs.requireType("/", Variable::DOUBLE | Variable::RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::DOUBLE:
			if (*rhs.doublePtr == 0)
				throw Exception("/: division by zero");
			return Variable(*lhs.doublePtr / *rhs.doublePtr);
		case Variable::RATIONAL:
			if (*rhs.rationalPtr == 0)
				throw Exception("/: division by zero");
			return Variable(*lhs.rationalPtr / *rhs.rationalPtr);
		default:
			double a = lhs.toDouble();
			double b = rhs.toDouble();
			if (b == 0)
				throw Exception("/: division by zero");
			return Variable(a / b);
	}
}

Variable operator-(const Variable& var)
{
	var.requireType("-", Variable::DOUBLE | Variable::RATIONAL);
	if (var.type == Variable::DOUBLE)
		return Variable(- *var.doublePtr);
	return Variable(- *var.rationalPtr);
}

// Compare operations

bool operator<(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("<", Variable::DOUBLE | Variable::RATIONAL);
	rhs.requireType("<", Variable::DOUBLE | Variable::RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::DOUBLE:
			return *lhs.doublePtr < *rhs.doublePtr;
		case Variable::RATIONAL:
			return *lhs.rationalPtr < *rhs.rationalPtr;
		default:
			return lhs.toDouble() < rhs.toDouble();
	}
}

bool operator>(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType(">", Variable::DOUBLE | Variable::RATIONAL);
	rhs.requireType(">", Variable::DOUBLE | Variable::RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::DOUBLE:
			return *lhs.doublePtr > *rhs.doublePtr;
		case Variable::RATIONAL:
			return *lhs.rationalPtr > *rhs.rationalPtr;
		default:
			return lhs.toDouble() > rhs.toDouble();
	}
}

bool operator<=(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("<=", Variable::DOUBLE | Variable::RATIONAL);
	rhs.requireType("<=", Variable::DOUBLE | Variable::RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::DOUBLE:
			return *lhs.doublePtr <= *rhs.doublePtr;
		case Variable::RATIONAL:
			return *lhs.rationalPtr <= *rhs.rationalPtr;
		default:
			return lhs.toDouble() <= rhs.toDouble();
	}
}

bool operator>=(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType(">=", Variable::DOUBLE | Variable::RATIONAL);
	rhs.requireType(">=", Variable::DOUBLE | Variable::RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::DOUBLE:
			return *lhs.doublePtr >= *rhs.doublePtr;
		case Variable::RATIONAL:
			return *lhs.rationalPtr >= *rhs.rationalPtr;
		default:
			return lhs.toDouble() >= rhs.toDouble();
	}
}

bool operator==(const Variable &lhs, const Variable &rhs)
{
	// Self compare
	if (lhs.refCount == rhs.refCount)
		return true;
	// Different type
	if (lhs.type != rhs.type)
		return false;
	switch (lhs.type) {
		case Variable::DOUBLE:
			return *lhs.doublePtr == *rhs.doublePtr;
		case Variable::RATIONAL:
			return *lhs.rationalPtr == *rhs.rationalPtr;
		case Variable::STRING:
		case Variable::SYMBOL:
			return *lhs.stringPtr == *rhs.stringPtr;
		case Variable::PAIR:
			return lhs.car() == rhs.car()
					&& lhs.cdr() == rhs.cdr();
		default:
			return false;
	}
}

bool operator!=(const Variable& lhs, const Variable& rhs)
{
	return !(lhs == rhs);
}

// Pair operations

Variable Variable::car() const
{
	requireType("car", Variable::PAIR);
	return pairPtr->first;
}

Variable Variable::cdr() const
{
	requireType("cdr", Variable::PAIR);
	return pairPtr->second;
}

Variable Variable::setCar(const Variable& var) const
{
	requireType("set-car!", Variable::PAIR);
	pairPtr->first = var;
	return VAR_VOID;
}

Variable Variable::setCdr(const Variable& var) const
{
	requireType("set-cdr!", Variable::PAIR);
	pairPtr->second = var;
	return VAR_VOID;
}

// Procedure operations

Variable Variable::operator()(const Variable& arg, const Environment &env)
{
	requireType("apply primitive procedure", PRIM);
	return primPtr->func(arg, env);
}

Variable Variable::getProcedureArgs() const
{
	requireType("get procedure args", COMP);
	return compPtr->args;
}

Variable Variable::getProcedureBody() const
{
	requireType("get procedure body", COMP);
	return compPtr->body;
}

string Variable::getProcedureName() const
{
	requireType("get procedure name", PRIM | COMP);
	if (type == Variable::PRIM)
		return primPtr->name;
	return compPtr->name;
}

Environment Variable::getProcedureEnv() const
{
	requireType("get procedure env", COMP);
	return compPtr->env;
}