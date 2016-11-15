//
// Low level variable and environment for Scheme
//
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include <iostream>
#include "variable.hpp"

// Type alias
using ostream = std::ostream;

// Standard output
ostream& operator<<(ostream& out, const Variable& var)
{
	switch (var.type) {
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

// Logical operations
Variable operator and(const Variable& lhs, const Variable& rhs)
{
	if (lhs != VAR_FALSE && rhs != VAR_FALSE)
		return VAR_TRUE;
	return VAR_FALSE;
}

Variable operator or(const Variable& lhs, const Variable& rhs)
{
	if (lhs != VAR_FALSE || rhs != VAR_FALSE)
		return VAR_TRUE;
	return VAR_FALSE;
}

Variable operator xor(const Variable& lhs, const Variable& rhs)
{

}

Variable operator not(const Variable& var)
{
	if (var == VAR_FALSE)
		return VAR_TRUE;
	return VAR_FALSE;
}

// Compare operations
Variable operator<(const Variable& lhs, const Variable& rhs)
{

}

Variable operator>(const Variable& lhs, const Variable& rhs)
{

}


bool operator!=(const Variable& lhs, const Variable& rhs)
{
	return !(lhs == rhs);
}

Variable operator<=(const Variable& lhs, const Variable& rhs)
{

}

Variable operator>=(const Variable& lhs, const Variable& rhs)
{

}

Variable Variable::operator()(const Variable& arg, const Environment &env)
{

}