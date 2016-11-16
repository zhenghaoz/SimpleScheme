//
// Low level variable and environment for Scheme
//
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include <iostream>
#include "variable.hpp"

// Constant values

const Variable VAR_NULL		= Variable();
const Variable VAR_VOID		= Variable();
const Variable VAR_TRUE		= Variable();
const Variable VAR_FALSE	= Variable();

// Type alias
using ostream = std::ostream;

// Standard output
ostream& operator<<(ostream& out, const Variable& var)
{
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
	}
	return out;
}

// Check operations

bool Variable::isNull() const
{
	return refCount == VAR_NULL.refCount;
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
			return lhs.pairPtr->first == rhs.pairPtr->first
					&& lhs.pairPtr->second == rhs.pairPtr->second;
		default:
			return false;
	}
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
	requireType("car", Variable::PAIR);
	pairPtr->first = var;
	return VAR_VOID;
}

Variable Variable::setCdr(const Variable& var) const
{
	requireType("car", Variable::PAIR);
	pairPtr->second = var;
	return VAR_VOID;
}

Variable Variable::operator()(const Variable& arg, const Environment &env)
{

}