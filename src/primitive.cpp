// 
// Primitive procedure and variable
// 
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include <boost/multiprecision/cpp_int.hpp>
#include "evaluator.hpp"
#include "primitive.hpp"

#define BOOL_TO_VAR(exp)		((exp) ? VAR_TRUE : VAR_FALSE)
#define FIRST_ARG(args)			((args).car())
#define SECOND_ARG(args)		((args).cdr().car())
#define REST_ARGS(args)			((args).cdr())

using boost::multiprecision::cpp_rational;

std::vector<Variable> prims = {

	Variable("eq?", [](const Variable& args, Environment &env)->Variable{
		Variable a = FIRST_ARG(args);
		Variable b = SECOND_ARG(args);
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
		Variable a = FIRST_ARG(args);
		if (REST_ARGS(args) == VAR_NULL)
			return -a;
		Variable b = SECOND_ARG(args);
		return a-b;
	}),

	Variable("/", [](const Variable& args, Environment &env)->Variable{
		Variable a = FIRST_ARG(args);
		Variable b = SECOND_ARG(args);
		return a/b;
	}),

	// Compare operations

	Variable("<", [](const Variable& args, Environment &env)->Variable{
		Variable a = FIRST_ARG(args);
		Variable b = SECOND_ARG(args);
		return BOOL_TO_VAR(a<b);
	}),

	Variable(">", [](const Variable& args, Environment &env)->Variable{
		Variable a = FIRST_ARG(args);
		Variable b = SECOND_ARG(args);
		return BOOL_TO_VAR(a>b);
	}),

	Variable("<=", [](const Variable& args, Environment &env)->Variable{
		Variable a = FIRST_ARG(args);
		Variable b = SECOND_ARG(args);
		return BOOL_TO_VAR(a<=b);
	}),

	Variable(">=", [](const Variable& args, Environment &env)->Variable{
		Variable a = FIRST_ARG(args);
		Variable b = SECOND_ARG(args);
		return BOOL_TO_VAR(a>=b);
	}),

	Variable("=", [](const Variable& args, Environment &env)->Variable{
		Variable a = FIRST_ARG(args);
		Variable b = SECOND_ARG(args);
		a.requireType("=", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
		b.requireType("=", Variable::TYPE_DOUBLE | Variable::TYPE_RATIONAL);
		return BOOL_TO_VAR(a==b);
	}),

	// Pair operations

	Variable("cons", [](const Variable& args, Environment &env)->Variable{
		Variable a = FIRST_ARG(args);
		Variable b = SECOND_ARG(args);
		return Variable(a, b);
	}),

	Variable("car", [](const Variable& args, Environment& env)->Variable{
		Variable pair = FIRST_ARG(args);
		return pair.car();
	}),

	Variable("cdr", [](const Variable& args, Environment& env)->Variable{
		Variable pair = FIRST_ARG(args);
		return pair.cdr();
	}),

	Variable("set-car!", [](const Variable& args, Environment& env)->Variable{
		Variable pair = FIRST_ARG(args);
		Variable car = SECOND_ARG(args);
		return pair.setCar(car);
	}),

	Variable("set-cdr!", [](const Variable& args, Environment& env)->Variable{
		Variable pair = FIRST_ARG(args);
		Variable cdr = SECOND_ARG(args);
		return pair.setCdr(cdr);
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

	// Advanced procedure

	Variable("eval", [](const Variable& args, Environment& env)->Variable{
		return eval(FIRST_ARG(args), env);
	}),

	Variable("apply", [](const Variable& args, Environment& env)->Variable{
		return apply(FIRST_ARG(args), SECOND_ARG(args), env);
	})
};

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
