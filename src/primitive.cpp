// 
// Primitive procedure and variable
// 
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include <list>
#include <cstdlib>
#include "evaluator.hpp"
#include "primitive.hpp"
#include "variable.hpp"
#include "exception.hpp"

#define BOOL_TO_VAR(exp)		((exp) ? VAR_TRUE : VAR_FALSE)
#define FIRST_ARG(args)			((args).car())
#define SECOND_ARG(args)		((args).cdr().car())
#define REST_ARGS(args)			((args).cdr())

using boost::multiprecision::cpp_rational;
using namespace Evaluator;
using namespace std;

namespace {

	std::vector<Variable> prims = {

		Variable("eq?", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return BOOL_TO_VAR(a == b);
		}),

		// Check operation

		Variable("null?", [](const Variable& args, Environment &env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args).isNull());
		}),

		Variable("number?", [](const Variable& args, Environment &env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args).isNumber());
		}),

		Variable("pair?", [](const Variable& args, Environment &env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args).isPair());
		}),

		Variable("string?", [](const Variable& args, Environment &env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args).isString());
		}),

		Variable("symbol?", [](const Variable& args, Environment &env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args).isSymbol());
		}),

		Variable("integer?", [](const Variable& args, Environment &env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args).isInteger());
		}),

		// Arithemtic operations

		Variable("+", [](const Variable& args, Environment &env)->Variable{
			Variable val = cpp_rational(0);
			for (Variable it = args; !it.isNull(); it = it.cdr()) {
				Variable a = it.car();
				val = val + a;
			}
			return val;
		}),

		Variable("*", [](const Variable& args, Environment &env)->Variable{
			Variable val = cpp_rational(1);
			for (Variable it = args; !it.isNull(); it = it.cdr()) {
				Variable a = it.car();
				val = val * a;
			}
			return val;
		}),

		Variable("-", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			if (REST_ARGS(args) == VAR_NULL)
				return -a;
			const Variable& b = SECOND_ARG(args);
			return a-b;
		}),

		Variable("/", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return a/b;
		}),

		Variable("remainder", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return remainder(a,b);
		}),

		Variable("quotient", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return quotient(a,b);
		}),

		Variable("random", [](const Variable& args, Environment& env)->Variable{
			const Variable& a = cpp_rational(rand());
			const Variable& b = FIRST_ARG(args);
			return remainder(a,b);
		}),

		Variable("gcd", [](const Variable& args, Environment& env)->Variable{
			const Variable& a = cpp_rational(rand());
			const Variable& b = FIRST_ARG(args);
			return gcd(a,b);
		}),

		Variable("even?", [](const Variable& args, Environment& env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args).isEven());
		}),

		Variable("odd?", [](const Variable& args, Environment& env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args).isOdd());
		}),

		// Compare operations

		Variable("<", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return BOOL_TO_VAR(a<b);
		}),

		Variable(">", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return BOOL_TO_VAR(a>b);
		}),

		Variable("<=", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return BOOL_TO_VAR(a<=b);
		}),

		Variable(">=", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return BOOL_TO_VAR(a>=b);
		}),

		Variable("=", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			a.requireType("=", Variable::TYPE_NUMBER);
			b.requireType("=", Variable::TYPE_NUMBER);
			return BOOL_TO_VAR(a==b);
		}),

		// Logical operations

		Variable("not", [](const Variable& args, Environment& env)->Variable{
			return BOOL_TO_VAR(FIRST_ARG(args) == VAR_FALSE);
		}),

		// Pair operations

		Variable("cons", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			return Variable(a, b);
		}),

		Variable("set-car!", [](const Variable& args, Environment& env)->Variable{
			const Variable& pair = FIRST_ARG(args);
			const Variable& car = SECOND_ARG(args);
			return pair.setCar(car);
		}),

		Variable("set-cdr!", [](const Variable& args, Environment& env)->Variable{
			const Variable& pair = FIRST_ARG(args);
			const Variable& cdr = SECOND_ARG(args);
			return pair.setCdr(cdr);
		}),

		Variable("car", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car();
		}),

		Variable("caar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().car();
		}),

		Variable("caaar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().car().car();
		}),

		Variable("caaaar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().car().car().car();
		}),

		Variable("caaadr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().car().car().cdr();
		}),

		Variable("caadr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().car().cdr();
		}),

		Variable("caadar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().car().cdr().car();
		}),

		Variable("caaddr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().car().cdr().cdr();
		}),

		Variable("cadr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().cdr();
		}),

		Variable("cadar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().cdr().car();
		}),

		Variable("cadaar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().cdr().car().car();
		}),

		Variable("cadadr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().cdr().car().cdr();
		}),

		Variable("caddr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().cdr().cdr();
		}),

		Variable("caddar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().cdr().cdr().car();
		}),

		Variable("cadddr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).car().cdr().cdr().cdr();
		}),

		Variable("cdr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr();
		}),

		Variable("cdar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().car();
		}),

		Variable("cdaar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().car().car();
		}),

		Variable("cdaaar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().car().car().car();
		}),

		Variable("cdaadr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().car().car().cdr();
		}),

		Variable("cdadr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().car().cdr();
		}),

		Variable("cdadar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().car().cdr().car();
		}),

		Variable("cdaddr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().car().cdr().cdr();
		}),

		Variable("cddr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().cdr();
		}),

		Variable("cddar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().cdr().car();
		}),

		Variable("cddaar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().cdr().car().car();
		}),

		Variable("cddadr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().cdr().car().cdr();
		}),

		Variable("cdddr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().cdr().cdr();
		}),

		Variable("cdddar", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().cdr().cdr().car();
		}),

		Variable("cddddr", [](const Variable& args, Environment& env)->Variable{
			return FIRST_ARG(args).cdr().cdr().cdr().cdr();
		}),

		// String operations

		Variable("number->string", [](const Variable& args, Environment& env)->Variable{
			const Variable num = FIRST_ARG(args);
			return Variable(num.toString(), Variable::TYPE_STRING);
		}),

		// List operations

		Variable("list", [](const Variable& args, Environment& env)->Variable{
			return args;
		}),

		Variable("length", [](const Variable& args, Environment& env)->Variable{
			cpp_rational count = 0;
			for (Variable it = FIRST_ARG(args); it != VAR_NULL; it = it.cdr())
				count++;
			return Variable(count);
		}),

		Variable("append", [](const Variable &args, Environment &env)->Variable{
			const Variable& head = Variable(VAR_NULL, VAR_NULL);
			Variable tail = head;
			for (Variable it = args; it != VAR_NULL; it = it.cdr()) {
				const Variable& li = it.car();
				if (li != VAR_NULL) {
					tail.setCdr(li);
					while (tail.cdr() != VAR_NULL)
						tail = tail.cdr();
				}
			}
			return head.cdr();
		}),

		Variable("map", [](const Variable &args, Environment &env)->Variable{
			Variable proc = args.car();
			// Get arguments
			list<Variable> argss;
			for (Variable it = args.cdr(); it != VAR_NULL; it = it.cdr())
				argss.push_back(it.car());
			// Apply
			Variable resultHead = Variable(VAR_NULL, VAR_NULL);
			Variable resultTail = resultHead;
			while (true) {
				Variable argsHead = Variable(VAR_NULL, VAR_NULL);
				Variable argsTail = argsHead;
				bool stop = false;
				for (auto it = argss.begin(); it != argss.end(); it++)
					if (it->isNull()) {	// Too few arguments
						stop = true;
						break;
					} else {			// Construct argument list
						Variable newTail = Variable(it->car(), VAR_NULL);
						argsTail.setCdr(newTail);
						argsTail = newTail;
						*it = it->cdr();
					}
				if (stop) {				// Too few arguments
					break;
				} else {				// Construct result list
					Variable newTail = Variable(apply(proc, argsHead.cdr(), env), VAR_NULL);
					resultTail.setCdr(newTail);
					resultTail = newTail;
				}
			}
			return resultHead.cdr();
		}),

		// I/O procedure

		Variable("display", [](const Variable& args, Environment& env)->Variable{
			std::cout << FIRST_ARG(args);
			return VAR_VOID;
		}),

		Variable("newline", [](const Variable& args, Environment& env)->Variable{
			std::cout << std::endl;
			return VAR_VOID;
		}),

		Variable("read", [](const Variable& args, Environment& env)->Variable{
			Variable val = VAR_NULL;
			if (std::cin >> val)
				return val;
			throw Exception("read: end of file");
		}),

		Variable("error", [](const Variable& args, Environment& env)->Variable{
			std::string msg;
			for (Variable it = args; !it.isNull(); it = it.cdr())
				msg += it.car().toString();
			throw Exception(msg);
		}),

		// Debug procedure

		Variable("assert", [](const Variable& args, Environment& env)->Variable{
			const Variable& a = FIRST_ARG(args);
			if (a == VAR_FALSE)
				throw Exception(string("assert:") + a.toString() + " isn't true");
			return VAR_VOID;
		}),

		Variable("assert=", [](const Variable& args, Environment &env)->Variable{
			const Variable& a = FIRST_ARG(args);
			const Variable& b = SECOND_ARG(args);
			if (a != b)
				throw Exception(string("assert:") + a.toString() + " != " + b.toString());
			return VAR_VOID;
		}),

		// Advanced procedure

		Variable("eval", [](const Variable& args, Environment& env)->Variable{
			return Evaluator::eval(FIRST_ARG(args), env);
		}),

		Variable("apply", [](const Variable& args, Environment& env)->Variable{
			return Evaluator::apply(FIRST_ARG(args), SECOND_ARG(args), env);
		})
	};

}

namespace Primitive {

	// Setup a base environment
	Environment setupEnvironment()
	{
		Environment env;
		// add constant 
		env.defineVariable("true", VAR_TRUE);
		env.defineVariable("false", VAR_FALSE);
		env.defineVariable("null", VAR_NULL);
		// add primitive procedure
		for (const Variable &prim : prims)
			env.defineVariable(prim.getProcedureName(), prim);
		return env;
	}

}