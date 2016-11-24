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
	if (var == VAR_NULL || var.type == Variable::TYPE_PAIR) {
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
		case Variable::TYPE_SPEC:
			if (var == VAR_TRUE)
				out << "#t";
			else if (var == VAR_FALSE)
				out << "#f";
			break;
		case Variable::TYPE_RATIONAL:
			out << *(var.rationalPtr);
			break;
		case Variable::TYPE_DOUBLE:
			out << *(var.doublePtr);
			break;
		case Variable::TYPE_STRING:
			out << '"' << *(var.stringPtr) << '"';
			break;
		case Variable::TYPE_SYMBOL:
			out << *(var.stringPtr);
			break;
		case Variable::TYPE_PRIM:
		case Variable::TYPE_COMP:
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
		case TYPE_DOUBLE:
			return "double";
		case TYPE_RATIONAL:
			return "rational";
		case TYPE_RATIONAL | TYPE_DOUBLE:
			return "number";
		case TYPE_PAIR:
			return "pair";
		case TYPE_STRING:
			return "string";
		case TYPE_SYMBOL:
			return "symbol";
		case TYPE_PRIM:
			return "primitive";
		case TYPE_COMP:
			return "compound";
		case TYPE_COMP | TYPE_PRIM:
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
	requireType("convert to double", TYPE_DOUBLE | TYPE_RATIONAL);
	if (type == TYPE_RATIONAL)
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
	return type == TYPE_PAIR;
}

bool Variable::isNumber() const
{
	return type & (TYPE_RATIONAL | TYPE_DOUBLE);
}

bool Variable::isSymbol() const
{
	return type == TYPE_SYMBOL;
}

bool Variable::isString() const
{
	return type == TYPE_STRING;
}

bool Variable::isProcedure() const
{
	return type & (TYPE_COMP | TYPE_PRIM);
}

// Arithmetic operations

Variable operator+(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("+", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	rhs.requireType("+", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_DOUBLE:
			return Variable(*lhs.doublePtr + *rhs.doublePtr);
		case Variable::TYPE_RATIONAL:
			return Variable(*lhs.rationalPtr + *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() + rhs.toDouble());
	}
}

Variable operator-(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("-", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	rhs.requireType("-", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_DOUBLE:
			return Variable(*lhs.doublePtr - *rhs.doublePtr);
		case Variable::TYPE_RATIONAL:
			return Variable(*lhs.rationalPtr - *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() - rhs.toDouble());
	}
}

Variable operator*(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("*", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	rhs.requireType("*", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_DOUBLE:
			return Variable(*lhs.doublePtr * *rhs.doublePtr);
		case Variable::TYPE_RATIONAL:
			return Variable(*lhs.rationalPtr * *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() * rhs.toDouble());
	}
}

Variable operator/(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("/", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	rhs.requireType("/", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_DOUBLE:
			if (*rhs.doublePtr == 0)
				throw Exception("/: division by zero");
			return Variable(*lhs.doublePtr / *rhs.doublePtr);
		case Variable::TYPE_RATIONAL:
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
	var.requireType("-", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	if (var.type == Variable::TYPE_DOUBLE)
		return Variable(- *var.doublePtr);
	return Variable(- *var.rationalPtr);
}

// Compare operations

bool operator<(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("<", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	rhs.requireType("<", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_DOUBLE:
			return *lhs.doublePtr < *rhs.doublePtr;
		case Variable::TYPE_RATIONAL:
			return *lhs.rationalPtr < *rhs.rationalPtr;
		default:
			return lhs.toDouble() < rhs.toDouble();
	}
}

bool operator>(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType(">", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	rhs.requireType(">", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_DOUBLE:
			return *lhs.doublePtr > *rhs.doublePtr;
		case Variable::TYPE_RATIONAL:
			return *lhs.rationalPtr > *rhs.rationalPtr;
		default:
			return lhs.toDouble() > rhs.toDouble();
	}
}

bool operator<=(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("<=", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	rhs.requireType("<=", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_DOUBLE:
			return *lhs.doublePtr <= *rhs.doublePtr;
		case Variable::TYPE_RATIONAL:
			return *lhs.rationalPtr <= *rhs.rationalPtr;
		default:
			return lhs.toDouble() <= rhs.toDouble();
	}
}

bool operator>=(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType(">=", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	rhs.requireType(">=", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_DOUBLE:
			return *lhs.doublePtr >= *rhs.doublePtr;
		case Variable::TYPE_RATIONAL:
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
		case Variable::TYPE_DOUBLE:
			return *lhs.doublePtr == *rhs.doublePtr;
		case Variable::TYPE_RATIONAL:
			return *lhs.rationalPtr == *rhs.rationalPtr;
		case Variable::TYPE_STRING:
		case Variable::TYPE_SYMBOL:
			return *lhs.stringPtr == *rhs.stringPtr;
		case Variable::TYPE_PAIR:
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
	requireType("car", Variable::TYPE_PAIR);
	return pairPtr->first;
}

Variable Variable::cdr() const
{
	requireType("cdr", Variable::TYPE_PAIR);
	return pairPtr->second;
}

Variable Variable::setCar(const Variable& var) const
{
	requireType("set-car!", Variable::TYPE_PAIR);
	pairPtr->first = var;
	return VAR_VOID;
}

Variable Variable::setCdr(const Variable& var) const
{
	requireType("set-cdr!", Variable::TYPE_PAIR);
	pairPtr->second = var;
	return VAR_VOID;
}

// Procedure operations

Variable Variable::operator()(const Variable& arg, const Environment &env)
{
	requireType("apply primitive procedure", TYPE_PRIM);
	return primPtr->func(arg, env);
}

Variable Variable::getProcedureArgs() const
{
	requireType("get procedure args", TYPE_COMP);
	return compPtr->args;
}

Variable Variable::getProcedureBody() const
{
	requireType("get procedure body", TYPE_COMP);
	return compPtr->body;
}

string Variable::getProcedureName() const
{
	requireType("get procedure name", TYPE_PRIM | TYPE_COMP);
	if (type == Variable::TYPE_PRIM)
		return primPtr->name;
	return compPtr->name;
}

Environment Variable::getProcedureEnv() const
{
	requireType("get procedure env", TYPE_COMP);
	return compPtr->env;
}