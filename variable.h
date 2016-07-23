#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "evaldef.h"

EVAL_NAMESPACE_BEGIN

/* debug code */

#ifdef EVAL_DEBUG

#define VAL_CREATE	do { _valueCreated++; } while (0)
#define VAL_DESTROYED	do { _valueDestroyed++; } while (0)
#define VAL_DEBUG_STATE	do { std::clog << "[value] created: " << Variable::_valueCreated << " destroyed: " << Variable::_valueDestroyed << std::endl; } while(0)

#else

#define VAL_CREATE	do {} while (0)
#define VAL_DESTROYED	do {} while (0)

#endif

class Variable
{
public:
	/* debug code */
	#ifdef EVAL_DEBUG
	static int _valueCreated;
	static int _valueDestroyed;
	#endif
	/* variable operator */
	friend std::ostream& operator<<(std::ostream& out, const Variable& var);
	friend Variable isNull(const Variable &var);
	friend Variable isNumber(const Variable &var);
	friend Variable isSymbol(const Variable &var);
	friend Variable isPair(const Variable &var);
	friend Variable isEqual(const Variable &a, const Variable &b);
	friend Variable operator+(const Variable &a, const Variable &b);
	friend Variable operator-(const Variable &a, const Variable &b);
	friend Variable operator*(const Variable &a, const Variable &b);
	friend Variable operator/(const Variable &a, const Variable &b);
	friend Variable operator<(const Variable &a, const Variable &b);
	friend Variable operator>(const Variable &a, const Variable &b);
	friend Variable operator==(const Variable &a, const Variable &b);
	friend Variable cons(const Variable &a, const Variable &b);
	friend Variable car(const Variable &var);
	friend Variable cdr(const Variable &var);
	friend Variable setCar(Variable pair, const Variable &var);
	friend Variable setCdr(Variable pair, const Variable &var);
public:
	using string = std::string;
	using pair = std::pair<Variable, Variable>;
private:
	static const char* _typeNames[];
	enum Type { NIL, NUMBER, SYMBOL, PAIR };
	union {
		void *_voidPtr;
		int *_intPtr;
		string *_strPtr;
		pair *_pairPtr;
	};
	int *_refCount;
	Type _type;
private:
	/* helper function */
	static void swap(Variable &a, Variable &b);
	static void printList(std::ostream& out, const Variable &var);
	Variable operator&&(const Variable &b);
public:
	/* interface */
	Variable(): _type(NIL), _refCount(new int(1)) {}
	Variable(int number): _intPtr(new int(number)), _type(NUMBER), _refCount(new int(1)) { VAL_CREATE; }
	Variable(const string &symbol): _strPtr(new string(symbol)), _type(SYMBOL), _refCount(new int(1)) { VAL_CREATE; }
	Variable(const Variable &a, const Variable &b): _pairPtr(new pair(a, b)), _type(PAIR), _refCount(new int(1)) { VAL_CREATE; }
	Variable(const Variable &var);
	Variable& operator=(Variable var);
	~Variable();
	void requireType(std::string caller, const Type type) const;
};

/* constants */
const Variable VAR_TRUE = Variable("true");
const Variable VAR_FALSE = Variable("false");

EVAL_NAMESPACE_END

#endif