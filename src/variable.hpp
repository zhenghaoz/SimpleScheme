//
// Variable for Scheme
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
#include "environment.hpp"

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
		PAIR 	 = 0x20,
		PRIM 	 = 0x40,
		COMP 	 = 0x80
	};
	
private:

	// Indent class
	struct Primitive;
	struct Compound;

	// Type alias
	using string = std::string;
	using ostream = std::ostream;
	using istream = std::istream;
	using ostringstream = std::ostringstream;
	using pair = std::pair<Variable, Variable>;
	using cpp_rational = boost::multiprecision::cpp_rational;
	using function = std::function<Variable(const Variable&, const Environment&)>;

	// Type of variable
	Type type;

	// Reference count
	int* refCount = nullptr;

	// Value of variable
	union {
		cpp_rational*	rationalPtr;
		double*		doublePtr;
		string*		stringPtr;
		pair*		pairPtr;
		void*		voidPtr;
		Primitive*	primPtr;
		Compound*	compPtr;
	};

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
	Variable(const Variable& lhs, const Variable& rhs): 
		type(PAIR), refCount(new int(1)), pairPtr(new pair(lhs, rhs)) {}

	// Constructor for primitive procedure
	Variable(const string& name, const function& func);

	// Constructor for compound procedure
	Variable(const string& name, const Variable& args, const Variable& body, const Environment& env);

	// Copy constructor
	Variable(const Variable& var): type(var.type), refCount(var.refCount), voidPtr(var.voidPtr)
	{
		// Increase reference count
		(*refCount)++;
	}

	// Destructor
	~Variable();

	// Swap two variables
	friend void swap(Variable& lhs, Variable& rhs);

	// Assignment
	Variable& operator=(Variable var)
	{
		swap(*this, var);
		return *this;
	}

	// Standard I/O
	friend ostream& operator<<(ostream& out, const Variable& var);
	friend istream& operator>>(istream& in, Variable& var);

	// Require type, throw exception if type is wrong
	void requireType(const string &caller, int type) const;

	// Get type name
	static string getTypeName(int type);

	// Convert operations
	string toString() const;
	double toDouble() const;

	// Check operations
	bool isNull() const;
	bool isVoid() const;
	bool isPair() const;
	bool isNumber() const;
	bool isSymbol() const;
	bool isString() const;
	bool isProcedure() const;

	// Arithmetic operations
	friend Variable operator+(const Variable& lhs, const Variable& rhs);
	friend Variable operator-(const Variable& lhs, const Variable& rhs);
	friend Variable operator*(const Variable& lhs, const Variable& rhs);
	friend Variable operator/(const Variable& lhs, const Variable& rhs);
	friend Variable operator-(const Variable& var);

	// Compare operations
	friend bool operator<(const Variable& lhs, const Variable& rhs);
	friend bool operator>(const Variable& lhs, const Variable& rhs);
	friend bool operator<=(const Variable& lhs, const Variable& rhs);
	friend bool operator>=(const Variable& lhs, const Variable& rhs);
	friend bool operator==(const Variable& lhs, const Variable& rhs);
	friend bool operator!=(const Variable& lhs, const Variable& rhs);

	// Pair operations
	Variable car() const;
	Variable cdr() const;
	Variable setCar(const Variable& var) const;
	Variable setCdr(const Variable& var) const;

	// Procedure operations
	Variable operator()(const Variable& arg, const Environment& env);
	Variable getProcedureArgs() const;
	Variable getProcedureBody() const;
	string getProcedureName() const;
	Environment getProcedureEnv() const;
};

// Primitive procedure

struct Variable::Primitive
{
	string name;	// Name of procedure
	function func;	// Function object
	Primitive(const string& name, const function& func): name(name), func(func) {}
};

// Compound procedure

struct Variable::Compound
{
	string name;		// Name of procedure
	Variable args, body;// Argument and body
	Environment env;	// Closure
	Compound(const string& name, const Variable& args, const Variable& body, const Environment& env):
		name(name), args(args), body(body), env(env) {}
};

// Inline functions

// Swap two variables
inline void swap(Variable& lhs, Variable& rhs)
{
	std::swap(lhs.type, rhs.type);
	std::swap(lhs.voidPtr, rhs.voidPtr);
	std::swap(lhs.refCount, rhs.refCount);
}

// Constructor for primitive procedure
inline Variable::Variable(const string& name, const function& func):
	type(PRIM), refCount(new int(1)), primPtr(new Primitive(name, func)) {}

// Constructor for compound procedure
inline Variable::Variable(const string& name, const Variable& args, const Variable& body, const Environment& env):
	type(COMP), refCount(new int(1)), compPtr(new Compound(name, args, body, env)) {}

// Destructor
inline Variable::~Variable() {
	// Decrease reference count
	(*refCount)--;
	if (*refCount > 0)
		return;
	// Free memory
	delete refCount;
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
			break;
		case PAIR:
			delete pairPtr;
			break;
		case PRIM:
			delete primPtr;
			break;
		case COMP:
			delete compPtr;		
			break;
		case SPEC:
			;
	}
}

// Constant values

extern const Variable VAR_NULL;
extern const Variable VAR_VOID;
extern const Variable VAR_TRUE;
extern const Variable VAR_FALSE;