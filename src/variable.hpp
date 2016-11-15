//
// Low level variable and environment for Scheme
//
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>
#include <boost/multiprecision/cpp_int.hpp>
#include "exception.hpp"

class Variable
{
public:

	// Type of variable
	enum Type {
		RATIONAL = 0x01,
		DOUBLE 	 = 0x02,
		STRING 	 = 0x04,
		SYMBOL   = 0x08,
		SPEC     = 0x10,
		PAIR 	 = 0x20
	};
	
private:

	// Indent class
	class Primitive;
	class Compound;
	class Environment;

	// Type alias
	using string = std::string;
	using ostream = std::ostream;
	using ostringstream = std::ostringstream;
	using pair = std::pair<Variable, Variable>;
	using cpp_rational = boost::multiprecision::cpp_rational;

	// Type of variable
	Type type;

	// Value of variable
	union {
		cpp_rational*	rationalPtr;
		double*		doublePtr;
		string*		stringPtr;
		pair*		pairPtr;
		void*		voidPtr;
	};

	// Reference count
	int* refCount = nullptr;

	void requireType(const string &caller, int type) const
	{

	}

	double toDouble() const
	{
		if (type == RATIONAL)
			return static_cast<double>(*rationalPtr);
		return *doublePtr;
	}

public:

	// Constructor for special
	Variable(): type(SPEC), refCount(new int(1)) {}

	// Constructor for rational
	Variable(const cpp_rational& rational): 
		type(RATIONAL), refCount(new int(1)), rationalPtr(new cpp_rational(rational)) {}

	// Constructor for double
	Variable(double value):
		type(DOUBLE), refCount(new int(1)), doublePtr(new double(value)) {}

	// Constructor for rational, double, string and symbol
	Variable(const string &str, Type type): type(type), refCount(new int(1))
	{
		switch (type) {
			// Convert string to rational
			case RATIONAL:
				rationalPtr = new cpp_rational(str);
				break;
			// Convert string to double
			case DOUBLE:
				doublePtr = new double(std::stod(str));
				break;
			case STRING:
			case SYMBOL:
				stringPtr = new string(str);
				break;
			default:
				throw Exception("intern error: variable construction error");
		}
	}

	// Constructor for pairs
	Variable(const Variable& lhs, Variable& rhs): 
		type(PAIR), refCount(new int(1)), pairPtr(new pair(lhs, rhs)) {}

	// Copy constructor
	Variable(const Variable& var): type(var.type), refCount(var.refCount), voidPtr(var.voidPtr)
	{
		// Increase reference count
		(*refCount)++;
	}

	// Destructor
	~Variable() {
		// Decrease reference count
		(*refCount)--;
		if (*refCount > 0)
			return;
		// Free memory
		switch (type) {
			case RATIONAL:
				delete rationalPtr;
				break;
			case DOUBLE:
				delete doublePtr;
				break;
			case STRING:
			case SYMBOL:
				delete stringPtr;
		}
	}

	// Assignment
	Variable& operator=(Variable var)
	{
		swap(*this, var);
		return *this;
	}

	// Get variable type
	Type getType() const
	{
		return type;
	}

	// Convert variable to string
	string toString() const
	{
		ostringstream out;
		out << (*this);
		return out.str();
	}

	// Standard output
	friend ostream& operator<<(ostream& out, const Variable& var);

	// Swap two variables
	friend void swap(Variable& lhs, Variable& rhs);

	// Arithmetic operations
	friend Variable operator+(const Variable& lhs, const Variable& rhs);
	friend Variable operator-(const Variable& lhs, const Variable& rhs);
	friend Variable operator*(const Variable& lhs, const Variable& rhs);
	friend Variable operator/(const Variable& lhs, const Variable& rhs);
	friend Variable operator-(const Variable& var);

	// Logical operations
	friend Variable andVar(const Variable& lhs, const Variable& rhs);
	friend Variable orVar(const Variable& lhs, const Variable& rhs);
	friend Variable xorVar(const Variable& lhs, const Variable& rhs);
	friend Variable notVar(const Variable& var);

	// Compare operations
	friend Variable operator<(const Variable& lhs, const Variable& rhs);
	friend Variable operator>(const Variable& lhs, const Variable& rhs);

	friend bool operator!=(const Variable& lhs, const Variable& rhs);
	friend Variable operator<=(const Variable& lhs, const Variable& rhs);
	friend Variable operator>=(const Variable& lhs, const Variable& rhs);

	// Function call
	Variable operator()(const Variable& arg, const Environment& env);

	bool operator==(const Variable &var) const {
		if (type != var.type)
			return false;
		switch (type) {
			case DOUBLE:
				return *doublePtr == *(var.doublePtr);
			case RATIONAL:
				return *rationalPtr == *(var.rationalPtr);
		}
	}
};

// Swap two variables
inline void swap(Variable& lhs, Variable& rhs)
{
	std::swap(lhs.type, rhs.type);
	std::swap(lhs.voidPtr, rhs.voidPtr);
	std::swap(lhs.refCount, rhs.refCount);
}

class Variable::Primitive
{
	string name;	// Name of procedure

public:
	Primitive();
	~Primitive();
};

class Variable::Compound
{
	string name;	// Name of procedure

public:
	Compound();
	~Compound();
};

// Constant values
const Variable VAR_NULL		= Variable();
const Variable VAR_VOID		= Variable();
const Variable VAR_TRUE		= Variable();
const Variable VAR_FALSE	= Variable();

/*class Environment
{
	// Type alias
	using frame = std::map<Variable, Variable>;
	template <typename T> using shared_ptr = std::shared_ptr<T>;

	// Data member
	shared_ptr<Environment> encloseEnvPtr;
	shared_ptr<frame> framePtr;

	void addVars(const Variable &vars, const Variable &vals);
	frame::iterator findVar(const Variable &var);
public:
	Environment(const Variable &vars = VAR_NULL, const Variable &vals = VAR_NULL);
	Environment(const Variable &vars, const Variable &vals, const Environment &encloseEnv);
	Variable assignVariable(const Variable &var, const Variable &val);
	Variable defineVariable(const Variable &var, const Variable &val);
	Variable lookupVariable(const Variable &var);
	void setTag(int tag);
	void finalize();
};*/