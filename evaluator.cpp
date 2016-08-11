#include <vector>
#include <stack>
#include <string>
#include <cmath>
#include <list>
#include <iostream>
#include "evaluator.h"
#include "variable.h"
#include "exception.h"

#define TAGGED_LIST(exp, tag)	(exp.isPair() && exp.car().isEqual(tag))
#define IS_SELF_EVALUATING(exp)	(exp.isNumber() || exp.isString())
#define IS_VARIABLE(exp)		exp.isSymbol()
#define IS_QUOTED(exp)			TAGGED_LIST(exp, "quote")
#define QUOTED(exp)				exp.cdr().car()
#define IS_DEFINE(exp)			TAGGED_LIST(exp, "define")
#define IS_DEFINE_VAR(exp)		(IS_DEFINE(exp) && exp.cdr().car().isSymbol())
#define IS_DEFINE_PROC(exp)		(IS_DEFINE(exp) && exp.cdr().car().isPair())
#define DEFINE_VAR_NAME(exp)	exp.cdr().car()
#define DEFINE_VAR_VAL(exp)		exp.cdr().cdr().car()
#define DEFINE_PROC_NAME(exp)	exp.cdr().car().car()
#define DEFINE_PROC_ARGS(exp)	exp.cdr().car().cdr()
#define DEFINE_PROC_BODY(exp)	exp.cdr().cdr()
#define IS_ASSIGNMENT(exp)		TAGGED_LIST(exp, "set!")
#define ASSIGNMENT_VAR(exp)		exp.cdr().car()
#define ASSIGNMENT_VAL(exp)		exp.cdr().cdr().car()
#define IS_SEQ(exp)				TAGGED_LIST(exp, "begin")
#define SEQUENCE(exp)			exp.cdr()
#define IS_LAMBDA(exp)			TAGGED_LIST(exp, "lambda")
#define LAMBDA_ARGS(exp)		exp.cdr().car()
#define LAMBDA_BODY(exp)		exp.cdr().cdr()
#define IS_AND(exp)				TAGGED_LIST(exp, "and")
#define AND_ARGS(exp)			exp.cdr()
#define IS_OR(exp)				TAGGED_LIST(exp, "or")
#define OR_ARGS(exp)			exp.cdr()
#define IS_IF(exp)				TAGGED_LIST(exp, "if")
#define IF_PRED(exp)			exp.cdr().car()
#define IF_CON(exp)				exp.cdr().cdr().car()
#define IF_ALTER(exp)			exp.cdr().cdr().cdr().car()
#define IS_APPLICATION(exp)		exp.isPair()
#define IS_COND(exp)			TAGGED_LIST(exp, "cond")
#define IS_ELSE(exp)			TAGGED_LIST(exp, "else")
#define COND_CLUASES(exp)		exp.cdr()
#define COND_PRED(exp)			exp.car()
#define COND_CONSEQUENCE(exp)	exp.cdr()
#define IS_LET(exp)				TAGGED_LIST(exp, "let")
#define LET_BINDINGS(exp)		exp.cdr().car()
#define BINDING_VAR(exp)		exp.car()
#define BINDING_VAL(exp)		exp.cdr().car()
#define LET_BODY(exp)			exp.cdr().cdr()
#define IS_EVAL(exp)			TAGGED_LIST(exp, "eval")
#define EVAL_EXP(exp)			exp.cdr().car()
#define IS_APPLY(exp)			TAGGED_LIST(exp, "apply")
#define APPLY_PROC(exp)			exp.cdr().car()
#define APPLY_ARGS(exp)			exp.cdr().cdr().car()
#define APPLICATION_NAME(exp)	exp.car()
#define APPLICATION_ARGS(exp)	exp.cdr()

EVAL_NAMESPACE_BEGIN

Variable evalAnd(const Variable &expr, Environment &env);
Variable evalOr(const Variable &expr, Environment &env);
Variable evalSeq(const Variable &expr, Environment &env);
Variable evalCond(const Variable &expr, Environment &env);
Variable evalLet(const Variable &expr, Environment &env);
Variable apply(const Variable &proc, const Variable &args, Environment &env);

std::vector<PrimitiveProcdeure> prims = {

	PrimitiveProcdeure("eq?", [](const Variable& args)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		return a.isEqual(b) ? VAR_TRUE : VAR_FALSE;
	}),

	PrimitiveProcdeure("null?", [](const Variable& args)->Variable{
		return args.car().isNull() ? VAR_TRUE : VAR_FALSE;
	}),

	PrimitiveProcdeure("+", [](const Variable& args)->Variable{
		number val = 0;
		for (Variable it = args; !it.isNull(); it = it.cdr()) {
			Variable a = it.car();
			a.requireType("+", Variable::NUMBER);
			val += number(a);
		}
		return val;
	}),

	PrimitiveProcdeure("*", [](const Variable& args)->Variable{
		number val = 1;
		for (Variable it = args; !it.isNull(); it = it.cdr()) {
			Variable a = it.car();
			a.requireType("*", Variable::NUMBER);
			val *= number(a);
		}
		return val;
	}),

	PrimitiveProcdeure("-", [](const Variable& args)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("-", Variable::NUMBER);
		b.requireType("-", Variable::NUMBER);
		return number(a) - number(b);
	}),

	PrimitiveProcdeure("/", [](const Variable& args)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("/", Variable::NUMBER);
		b.requireType("/", Variable::NUMBER);
		if (number(b) == 0)
			throw SchemeException("/: divide by zero");
		return number(a) / number(b);
	}),

	PrimitiveProcdeure("<", [](const Variable& args)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("<", Variable::NUMBER);
		b.requireType("<", Variable::NUMBER);
		return number(a) < number(b) ? VAR_TRUE : VAR_FALSE;
	}),

	PrimitiveProcdeure(">", [](const Variable& args)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType(">", Variable::NUMBER);
		b.requireType(">", Variable::NUMBER);
		return number(a) > number(b) ? VAR_TRUE : VAR_FALSE;
	}),

	PrimitiveProcdeure("=", [](const Variable& args)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("=", Variable::NUMBER);
		b.requireType("=", Variable::NUMBER);
		return number(a) == number(b) ? VAR_TRUE : VAR_FALSE;
	}),

	PrimitiveProcdeure("remainder", [](const Variable &args)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("remainder", Variable::NUMBER);
		b.requireType("remainder", Variable::NUMBER);
		return number(a) % number(b);
	}),

	PrimitiveProcdeure("cons", [](const Variable& args)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		return Variable(a, b);
	}),

	PrimitiveProcdeure("car", [](const Variable &args)->Variable{
		Variable pair = args.car();
		return pair.car();
	}),

	PrimitiveProcdeure("cdr", [](const Variable &args)->Variable{
		Variable pair = args.car();
		return pair.cdr();
	}),

	PrimitiveProcdeure("set-car!", [](const Variable &args)->Variable{
		Variable pair = args.car();
		Variable car = args.cdr().car();
		return pair.setCar(car);
	}),

	PrimitiveProcdeure("set-cdr!", [](const Variable &args)->Variable{
		Variable pair = args.car();
		Variable cdr = args.cdr().car();
		return pair.setCdr(cdr);
	}),

	PrimitiveProcdeure("list", [](const Variable &args)->Variable{
		return args;
	}),

	PrimitiveProcdeure("map", [](const Variable &args)->Variable{
		Variable proc = args.car();
		std::list<Variable> argss;
		for (Variable it = args.cdr(); !it.isNull(); it = it.cdr())
			argss.push_back(it.car());
	})
};

Variable eval(const Variable &expr, Environment &env)
{
	if (IS_SELF_EVALUATING(expr))
		return expr;
	if (IS_VARIABLE(expr))
		return env.lookupVariable(expr);
	if (IS_QUOTED(expr))
		return QUOTED(expr);
	if (IS_DEFINE_VAR(expr))
		return env.defineVariable(DEFINE_VAR_NAME(expr), eval(DEFINE_VAR_VAL(expr), env));
	if (IS_DEFINE_PROC(expr))
		return env.defineVariable(DEFINE_PROC_NAME(expr), 
				Variable(DEFINE_PROC_NAME(expr),
					DEFINE_PROC_ARGS(expr),
					DEFINE_PROC_BODY(expr), env));
	if (IS_ASSIGNMENT(expr))
		return env.assignVariable(ASSIGNMENT_VAR(expr), eval(ASSIGNMENT_VAL(expr), env));
	if (IS_SEQ(expr))
		return evalSeq(SEQUENCE(expr), env);
	if (IS_AND(expr))
		return evalAnd(expr, env);
	if (IS_OR(expr))
		return evalOr(expr, env);
	if (IS_IF(expr))
		return eval(IF_PRED(expr), env) ? eval(IF_CON(expr), env) : eval(IF_ALTER(expr), env);
	if (IS_COND(expr))
		return evalCond(expr, env);
	if (IS_LAMBDA(expr))
		return Variable(LAMBDA_ARGS(expr), LAMBDA_BODY(expr), env);
	if (IS_LET(expr))
		return evalLet(expr, env);
	if (IS_EVAL(expr))
		return eval(eval(EVAL_EXP(expr), env), env);
	if (IS_APPLY(expr))
		return apply(APPLY_PROC(expr), eval(APPLY_ARGS(expr), env), env);
	if (IS_APPLICATION(expr))
		return apply(APPLICATION_NAME(expr), APPLICATION_ARGS(expr), env);
	throw SchemeException(std::string("eval: can't evaluate ") + expr.toString());
}

Variable evalAnd(const Variable &expr, Environment &env)
{
	for (Variable it = AND_ARGS(expr); !it.isNull(); it = it.cdr())
		if (!eval(it.car(), env))
			return VAR_FALSE;
	return VAR_TRUE;
}

Variable evalOr(const Variable &expr, Environment &env)
{
	for (Variable it = OR_ARGS(expr); !it.isNull(); it = it.cdr())
		if (eval(it.car(), env))
			return VAR_TRUE;
	return VAR_FALSE;
}

Variable evalSeq(const Variable &expr, Environment &env)
{
	Variable val = VAR_VOID;
	for (Variable it = expr; !it.isNull(); it = it.cdr())
		val = eval(it.car(), env);
	return val;
}

Variable evalCond(const Variable &expr, Environment &env)
{
	for (Variable clauses = COND_CLUASES(expr); !clauses.isNull(); clauses = clauses.cdr()) {
		Variable clause = clauses.car();
		if (IS_ELSE(clause) || eval(COND_PRED(clause), env))
			return evalSeq(COND_CONSEQUENCE(clause), env);
	}
	return VAR_VOID;
}

Variable evalLet(const Variable &expr, Environment &env)
{
	Environment extendEnv = Environment(VAR_NULL, VAR_NULL, env);
	for (Variable bindings = LET_BINDINGS(expr); !bindings.isNull(); bindings = bindings.cdr()) {
		Variable binding = bindings.car();
		extendEnv.defineVariable(BINDING_VAR(binding), BINDING_VAL(binding));
	}
	return evalSeq(LET_BODY(expr), extendEnv);
}

Variable apply(const Variable &proc, const Variable &args, Environment &env)
{
	Variable procEvaled = eval(proc, env);
	std::stack<Variable> argStack;
	for (Variable it = args; !it.isNull(); it = it.cdr())
		argStack.push(eval(it.car(), env));
	Variable vals = VAR_NULL;
	while (!argStack.empty()) {
		Variable val = argStack.top();
		vals = Variable(val, vals);
		argStack.pop();
	}
	if (procEvaled.isPrim())
		return procEvaled(vals);
	if (procEvaled.isComp()) {
		CompoundProcedure comp = CompoundProcedure(procEvaled);
		Variable body = comp.getSequence();
		Variable args = comp.getArguments();
		Environment extendEnv = Environment(args, vals, comp.getEnvironmrnt());
		return evalSeq(body, extendEnv);
	}
	throw SchemeException(std::string("eval: can't apply ") + proc.toString());
}

Environment setupEnvironment()
{
	Environment env = Environment();
	GarbageCollector::setGlobalEnvironment(env);
	env.defineVariable(std::string("true"), VAR_TRUE);
	env.defineVariable(std::string("false"), VAR_FALSE);
	env.defineVariable(std::string("null"), VAR_NULL);
	for (const PrimitiveProcdeure &prim : prims)
		env.defineVariable(prim.getName(), prim);
	return env;
}

EVAL_NAMESPACE_END