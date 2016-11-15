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

}

Variable operator-(const Variable& lhs, const Variable& rhs)
{

}

Variable operator*(const Variable& lhs, const Variable& rhs)
{

}

Variable operator/(const Variable& lhs, const Variable& rhs)
{

}

Variable operator-(const Variable& var)
{

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

bool operator==(const Variable& lhs, const Variable& rhs)
{
	return lhs.refCount == rhs.refCount;
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