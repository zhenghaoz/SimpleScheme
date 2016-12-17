//
// Variable for Scheme
//
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include <boost/multiprecision/cpp_int.hpp>
#include "variable.hpp"

#ifdef STATS
#include "statistic.hpp"
#endif

// Constant values
const Variable VAR_NULL		= Variable();
const Variable VAR_VOID		= Variable();
const Variable VAR_TRUE		= Variable();
const Variable VAR_FALSE	= Variable();

using namespace std;
using namespace boost::multiprecision;

// Parser
extern int yyparse(std::istream* in, std::ostream* out = 0);
extern Variable yypval;

// Constructors

// Constructor for special
Variable::Variable(): type(TYPE_SPEC), refCount(new int(1)) 
{
	#ifdef STATS
	Statistic::createVariable();
	#endif
}

// Constructor for rational
Variable::Variable(const cpp_rational& rational): 
	type(TYPE_RATIONAL), refCount(new int(1)), rationalPtr(new cpp_rational(rational))
{
	#ifdef STATS
	Statistic::createVariable();
	#endif
}

// Constructor for double
Variable::Variable(double value):
	type(TYPE_FLOAT), refCount(new int(1)), doublePtr(new double(value))
{
	#ifdef STATS
	Statistic::createVariable();
	#endif
}

// Constructor for rational, double, string and symbol
Variable::Variable(const string &str, Type type): type(type), refCount(new int(1))
{
	switch (type) {
		// Convert string to rational
		case TYPE_RATIONAL:
			rationalPtr = new cpp_rational(str);
			break;
		// Convert string to double
		case TYPE_FLOAT:
			doublePtr = new double(stod(str));
			break;
		case TYPE_SYMBOL:
		case TYPE_STRING:
			stringPtr = new string(str);
			break;
		default:
			throw Exception("intern error: variable construction error");
	}
	#ifdef STATS
	Statistic::createVariable();
	#endif
}

// Constructor for primitive procedure
Variable::Variable(const string& name, const function& func):
	type(TYPE_PRIM), refCount(new int(1)), primPtr(new Primitive(name, func))
{
	#ifdef STATS
	Statistic::createVariable();
	#endif
}

// Constructor for compound procedure
Variable::Variable(const string& name, const Variable& args, const Variable& body, const Environment& env):
	type(TYPE_COMP), refCount(new int(1)), compPtr(new Compound(name, args, body, env))
{
	GarbageCollector::trace(*this);
	#ifdef STATS
	Statistic::createVariable();
	#endif
}

// Constructor for pairs
Variable::Variable(const Variable& lhs, const Variable& rhs): 
	type(TYPE_PAIR), refCount(new int(1)), pairPtr(new pair(lhs, rhs))
{
	GarbageCollector::trace(*this);
	#ifdef STATS
	Statistic::createVariable();
	#endif
}

// Copy constructor
Variable::Variable(const Variable& var): 
	type(var.type), refCount(var.refCount), voidPtr(var.voidPtr), GarbageObject(var)
{
	#ifdef STATS
	Statistic::copyVariable();
	#endif
	// Increase reference count
	(*refCount)++;
}

// Destructor
Variable::~Variable() {
	// Decrease reference count
	(*refCount)--;
	if (*refCount > 0)
		return;
	// Free memory
	delete refCount;
	switch (type) {
		case TYPE_RATIONAL:
			delete rationalPtr;
			break;
		case TYPE_FLOAT:
			delete doublePtr;
			break;
		case TYPE_STRING:
		case TYPE_SYMBOL:
			delete stringPtr;
			break;
		case TYPE_PAIR:
			delete pairPtr;
			break;
		case TYPE_PRIM:
			delete primPtr;
			break;
		case TYPE_COMP:
			delete compPtr;		
			break;
		default:
			;
	}
	#ifdef STATS
	Statistic::destroyVariable();
	#endif
}

// Assignment
Variable& Variable::operator=(Variable var)
{
	swap(*this, var);
	return *this;
}

// Swap two variables
void swap(Variable& lhs, Variable& rhs)
{
	std::swap(static_cast<GarbageObject&>(lhs), static_cast<GarbageObject&>(rhs));
	std::swap(lhs.type, rhs.type);
	std::swap(lhs.voidPtr, rhs.voidPtr);
	std::swap(lhs.refCount, rhs.refCount);
}

// Standard I/0

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
		case Variable::TYPE_FLOAT:
			out << *(var.doublePtr);
			break;
		case Variable::TYPE_STRING:
			out << *(var.stringPtr);
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

istream& operator>>(istream& in, Variable& var)
{
	yyparse(&in);
	var = yypval;
	return in;
}

// Require type

void Variable::requireType(const string &caller, Type type) const
{
	if (this->type & type)
		return;
	switch (type) {
		case TYPE_INTEGER:
			if (isInteger())
				return;
			break;
		default:
			;
	}
	throw Exception(caller + ": contract violation\n" 
		+ "\texpected: " + getTypeName(type) + "\n"
		+ "\tgiven: " + this->toString());
}

// Get type name

string Variable::getTypeName(Type type)
{
	switch (type) {
		case TYPE_FLOAT:
			return "double";
		case TYPE_RATIONAL:
			return "rational";
		case TYPE_NUMBER:
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
		case TYPE_PROCEDURE:
			return "procedure";
		case TYPE_INTEGER:
			return "integer";
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
	requireType("convert to double", TYPE_NUMBER);
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
	return type & (TYPE_NUMBER);
}

bool Variable::isInteger() const
{
	return type == TYPE_RATIONAL &&
		denominator(*rationalPtr) == 1;
}

bool Variable::isSymbol() const
{
	return type == TYPE_SYMBOL;
}

bool Variable::isString() const
{
	return type == TYPE_STRING;
}

bool Variable::isPrim() const
{
	return type == TYPE_PRIM;
}	

bool Variable::isComp() const
{
	return type == TYPE_COMP;
}

bool Variable::isProcedure() const
{
	return type & (TYPE_COMP | TYPE_PRIM);
}

// Arithmetic operations

Variable operator+(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("+", Variable::TYPE_NUMBER);
	rhs.requireType("+", Variable::TYPE_NUMBER);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_FLOAT:
			return Variable(*lhs.doublePtr + *rhs.doublePtr);
		case Variable::TYPE_RATIONAL:
			return Variable(*lhs.rationalPtr + *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() + rhs.toDouble());
	}
}

Variable operator-(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("-", Variable::TYPE_NUMBER);
	rhs.requireType("-", Variable::TYPE_NUMBER);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_FLOAT:
			return Variable(*lhs.doublePtr - *rhs.doublePtr);
		case Variable::TYPE_RATIONAL:
			return Variable(*lhs.rationalPtr - *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() - rhs.toDouble());
	}
}

Variable operator*(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("*", Variable::TYPE_NUMBER);
	rhs.requireType("*", Variable::TYPE_NUMBER);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_FLOAT:
			return Variable(*lhs.doublePtr * *rhs.doublePtr);
		case Variable::TYPE_RATIONAL:
			return Variable(*lhs.rationalPtr * *rhs.rationalPtr);
		default:
			return Variable(lhs.toDouble() * rhs.toDouble());
	}
}

Variable operator/(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("/", Variable::TYPE_NUMBER);
	rhs.requireType("/", Variable::TYPE_NUMBER);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_FLOAT:
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
	var.requireType("-", Variable::TYPE_NUMBER);
	if (var.type == Variable::TYPE_FLOAT)
		return Variable(- *var.doublePtr);
	return Variable(- *var.rationalPtr);
}

Variable remainder(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("remainder", Variable::TYPE_INTEGER);
	rhs.requireType("remainder", Variable::TYPE_INTEGER);
	const cpp_int& a = numerator(*lhs.rationalPtr);
	const cpp_int& b = numerator(*rhs.rationalPtr);
	return Variable(cpp_rational(a%b));
}

Variable quotient(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("quotient", Variable::TYPE_INTEGER);
	rhs.requireType("quotient", Variable::TYPE_INTEGER);
	const cpp_int& a = numerator(*lhs.rationalPtr);
	const cpp_int& b = numerator(*rhs.rationalPtr);
	return Variable(cpp_rational(a/b));
}

Variable gcd(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("gcd", Variable::TYPE_INTEGER);
	rhs.requireType("gcd", Variable::TYPE_INTEGER);
	cpp_int a = numerator(*lhs.rationalPtr);
	cpp_int b = numerator(*rhs.rationalPtr);
	while (b) {
		cpp_int temp = a;
		a = b;
		b = temp % b;
	}
	return Variable(cpp_rational(a));
}

bool Variable::isEven() const
{
	requireType("remainder", Variable::TYPE_INTEGER);
	const cpp_int& a = numerator(*rationalPtr);
	return a % 2 == 0;
}

bool Variable::isOdd() const
{
	requireType("remainder", Variable::TYPE_INTEGER);
	const cpp_int& a = numerator(*rationalPtr);
	return a % 2 == 1;
}

// Compare operations

bool operator<(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("<", Variable::TYPE_NUMBER);
	rhs.requireType("<", Variable::TYPE_NUMBER);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_FLOAT:
			return *lhs.doublePtr < *rhs.doublePtr;
		case Variable::TYPE_RATIONAL:
			return *lhs.rationalPtr < *rhs.rationalPtr;
		default:
			return lhs.toDouble() < rhs.toDouble();
	}
}

bool operator>(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType(">", Variable::TYPE_NUMBER);
	rhs.requireType(">", Variable::TYPE_NUMBER);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_FLOAT:
			return *lhs.doublePtr > *rhs.doublePtr;
		case Variable::TYPE_RATIONAL:
			return *lhs.rationalPtr > *rhs.rationalPtr;
		default:
			return lhs.toDouble() > rhs.toDouble();
	}
}

bool operator<=(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType("<=", Variable::TYPE_NUMBER);
	rhs.requireType("<=", Variable::TYPE_NUMBER);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_FLOAT:
			return *lhs.doublePtr <= *rhs.doublePtr;
		case Variable::TYPE_RATIONAL:
			return *lhs.rationalPtr <= *rhs.rationalPtr;
		default:
			return lhs.toDouble() <= rhs.toDouble();
	}
}

bool operator>=(const Variable& lhs, const Variable& rhs)
{
	lhs.requireType(">=", Variable::TYPE_NUMBER);
	rhs.requireType(">=", Variable::TYPE_NUMBER);
	switch (lhs.type | rhs.type) {
		case Variable::TYPE_FLOAT:
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
		case Variable::TYPE_FLOAT:
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

Variable& Variable::car() const
{
	requireType("car", Variable::TYPE_PAIR);
	return pairPtr->first;
}

Variable& Variable::cdr() const
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

Variable Variable::operator()(const Variable& arg, Environment &env) const
{
	requireType("apply primitive procedure", TYPE_PRIM);
	return primPtr->func(arg, env);
}

Variable& Variable::getProcedureArgs() const
{
	requireType("get procedure args", TYPE_COMP);
	return compPtr->args;
}

Variable& Variable::getProcedureBody() const
{
	requireType("get procedure body", TYPE_COMP);
	return compPtr->body;
}

string Variable::getProcedureName() const
{
	requireType("get procedure name", TYPE_PROCEDURE);
	if (type == Variable::TYPE_PRIM)
		return primPtr->name;
	return compPtr->name;
}

Environment Variable::getProcedureEnv() const
{
	requireType("get procedure env", TYPE_COMP);
	return compPtr->env;
}

// Optimization: constant pool

std::unordered_map<std::string, Variable> Variable::pool;

Variable Variable::createSymbol(const std::string& str)
{
	if (pool.find(str) == pool.end())
		pool[str] = Variable(str, Variable::TYPE_SYMBOL);
	return pool[str];
}

// Optimization: garbage collection

void Variable::finalize() const
{
	switch (type) {
		case TYPE_PAIR:
			pairPtr->first = VAR_NULL;
			pairPtr->second = VAR_NULL;
			break;
		case TYPE_COMP:
			compPtr->args = VAR_NULL;
			compPtr->body = VAR_NULL;
			compPtr->env = Environment();
		default:
			;
	};
}

void Variable::scan(int tag) const
{
	switch (type) {
		case TYPE_PAIR:
			*gcTag = tag;
			if (*pairPtr->first.gcTag != tag)
				pairPtr->first.scan(tag);
			if (*pairPtr->second.gcTag != tag)
				pairPtr->second.scan(tag);
			break;
		case TYPE_COMP:
			*gcTag = tag;
			if (*compPtr->args.gcTag != tag)
				compPtr->args.scan(tag);
			if (*compPtr->body.gcTag != tag)
				compPtr->body.scan(tag);
			if (*compPtr->env.gcTag != tag)
				compPtr->env.scan(tag);
			break;
		default:
			;
	}
}