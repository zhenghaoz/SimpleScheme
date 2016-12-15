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
#include <unordered_map>
#include <boost/multiprecision/cpp_int.hpp>
#include "exception.hpp"
#include "environment.hpp"
#include "garbage.hpp"

class Variable: public GarbageObject
{
public:

	// Type of variable
	enum Type {
		TYPE_RATIONAL 	= 0x01,
		TYPE_FLOAT 		= 0x02,
		TYPE_STRING 	= 0x04,
		TYPE_SYMBOL  	= 0x08,
		TYPE_SPEC     	= 0x10,
		TYPE_PAIR 	 	= 0x20,
		TYPE_PRIM 	 	= 0x40,
		TYPE_COMP 	 	= 0x80,
		// Type class
		TYPE_TEXT		= 0x0C,
		TYPE_NUMBER		= 0x03,
		TYPE_PROCEDURE	= 0xC0,
		// Sub type
		TYPE_INTEGER	= 0x100
	};
	
private:

	// Indent class
	struct Primitive;
	struct Compound;
	friend Environment;

	// Type alias
	using string = std::string;
	using ostream = std::ostream;
	using istream = std::istream;
	using ostringstream = std::ostringstream;
	using pair = std::pair<Variable, Variable>;
	using cpp_rational = boost::multiprecision::cpp_rational;
	using function = std::function<Variable(const Variable&, Environment&)>;

	// Optimization: constant pool
	static std::unordered_map<std::string, Variable> pool;

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
	Variable();

	// Constructor for rational
	Variable(const cpp_rational& rational);

	// Constructor for double
	Variable(double value);

	// Constructor for rational, double, string and symbol
	Variable(const string &str, Type type);

	// Constructor for pairs
	Variable(const Variable& lhs, const Variable& rhs);

	// Constructor for primitive procedure
	Variable(const string& name, const function& func);

	// Constructor for compound procedure
	Variable(const string& name, const Variable& args, const Variable& body, const Environment& env);

	// Copy constructor
	Variable(const Variable& var);

	// Destructor
	~Variable();

	// Swap two variables
	friend void swap(Variable& lhs, Variable& rhs);

	// Assignment
	Variable& operator=(Variable var);

	// Optimization: constant pool
	static Variable createSymbol(const std::string& str);

	// Finalize value
	void finalize() const override;

	// Scan and tag value in using
	void scan(int tag) const override;

	// Standard I/O
	friend ostream& operator<<(ostream& out, const Variable& var);
	friend istream& operator>>(istream& in, Variable& var);

	// Require type, throw exception if type is wrong
	void requireType(const string &caller, Type type) const;

	// Get type name
	static string getTypeName(Type type);

	// Convert operations
	string toString() const;
	double toDouble() const;

	// Check operations
	bool isNull() const;
	bool isVoid() const;
	bool isPair() const;
	bool isNumber() const;
	bool isInteger() const;
	bool isSymbol() const;
	bool isString() const;
	bool isPrim() const;
	bool isComp() const;
	bool isProcedure() const;

	// Arithmetic operations
	friend Variable operator+(const Variable& lhs, const Variable& rhs);
	friend Variable operator-(const Variable& lhs, const Variable& rhs);
	friend Variable operator*(const Variable& lhs, const Variable& rhs);
	friend Variable operator/(const Variable& lhs, const Variable& rhs);
	friend Variable operator-(const Variable& var);
	friend Variable remainder(const Variable& lhs, const Variable& rhs);
	friend Variable quotient(const Variable& lhs, const Variable& rhs);
	bool isEven() const;
	bool isOdd() const;

	// Compare operations
	friend bool operator<(const Variable& lhs, const Variable& rhs);
	friend bool operator>(const Variable& lhs, const Variable& rhs);
	friend bool operator<=(const Variable& lhs, const Variable& rhs);
	friend bool operator>=(const Variable& lhs, const Variable& rhs);
	friend bool operator==(const Variable& lhs, const Variable& rhs);
	friend bool operator!=(const Variable& lhs, const Variable& rhs);

	// Pair operations
	Variable& car() const;
	Variable& cdr() const;
	Variable setCar(const Variable& var) const;
	Variable setCdr(const Variable& var) const;

	// Procedure operations
	Variable operator()(const Variable& arg, Environment& env) const;
	Variable& getProcedureArgs() const;
	Variable& getProcedureBody() const;
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

// Constant values

extern const Variable VAR_NULL;
extern const Variable VAR_VOID;
extern const Variable VAR_TRUE;
extern const Variable VAR_FALSE;
