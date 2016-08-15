#include <vector>
#include <string>
#include <cmath>
#include <list>
#include <iostream>
#include "evaluator.h"
#include "variable.h"
#include "exception.h"
#include "parser.h"

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
#define APPLICATION_NAME(exp)	exp.car()
#define APPLICATION_ARGS(exp)	exp.cdr()

#define BOOL_TO_VAR(exp)		(exp ? VAR_TRUE : VAR_FALSE)

EVAL_NAMESPACE_BEGIN

Variable evalAnd(const Variable &expr, Environment &env);
Variable evalOr(const Variable &expr, Environment &env);
Variable evalSeq(const Variable &expr, Environment &env);
Variable evalCond(const Variable &expr, Environment &env);
Variable evalLet(const Variable &expr, Environment &env);
Variable evalArgs(const Variable &args, Environment &env);
Variable apply(const Variable &proc, const Variable &args, Environment &env);

std::vector<PrimitiveProcdeure> prims = {

	PrimitiveProcdeure("eq?", [](const Variable& args, Environment &env)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		return BOOL_TO_VAR(a.isEqual(b));
	}),

	PrimitiveProcdeure("null?", [](const Variable& args, Environment &env)->Variable{
		return BOOL_TO_VAR(args.car().isNull());
	}),

	PrimitiveProcdeure("number?", [](const Variable& args, Environment &env)->Variable{
		return BOOL_TO_VAR(args.car().isNumber());
	}),

	PrimitiveProcdeure("pair?", [](const Variable& args, Environment &env)->Variable{
		return BOOL_TO_VAR(args.car().isPair());
	}),

	PrimitiveProcdeure("string?", [](const Variable& args, Environment &env)->Variable{
		return BOOL_TO_VAR(args.car().isString());
	}),

	PrimitiveProcdeure("symbol?", [](const Variable& args, Environment &env)->Variable{
		return BOOL_TO_VAR(args.car().isSymbol());
	}),

	PrimitiveProcdeure("+", [](const Variable& args, Environment &env)->Variable{
		number val = 0;
		for (Variable it = args; !it.isNull(); it = it.cdr()) {
			Variable a = it.car();
			a.requireType("+", Variable::NUMBER);
			val += static_cast<number>(a);
		}
		return val;
	}),

	PrimitiveProcdeure("*", [](const Variable& args, Environment &env)->Variable{
		number val = 1;
		for (Variable it = args; !it.isNull(); it = it.cdr()) {
			Variable a = it.car();
			a.requireType("*", Variable::NUMBER);
			val *= static_cast<number>(a);
		}
		return val;
	}),

	PrimitiveProcdeure("-", [](const Variable& args, Environment &env)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("-", Variable::NUMBER);
		b.requireType("-", Variable::NUMBER);
		return static_cast<number>(a) - static_cast<number>(b);
	}),

	PrimitiveProcdeure("/", [](const Variable& args, Environment &env)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("/", Variable::NUMBER);
		b.requireType("/", Variable::NUMBER);
		if (static_cast<number>(b) == 0)
			throw SchemeException("/: divide by zero");
		return static_cast<number>(a) / static_cast<number>(b);
	}),

	PrimitiveProcdeure("<", [](const Variable& args, Environment &env)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("<", Variable::NUMBER);
		b.requireType("<", Variable::NUMBER);
		return BOOL_TO_VAR(static_cast<number>(a) < static_cast<number>(b));
	}),

	PrimitiveProcdeure(">", [](const Variable& args, Environment &env)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType(">", Variable::NUMBER);
		b.requireType(">", Variable::NUMBER);
		return BOOL_TO_VAR(static_cast<number>(a) > static_cast<number>(b));
	}),

	PrimitiveProcdeure("=", [](const Variable& args, Environment &env)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("=", Variable::NUMBER);
		b.requireType("=", Variable::NUMBER);
		return BOOL_TO_VAR(static_cast<number>(a) == static_cast<number>(b));
	}),

	PrimitiveProcdeure("remainder", [](const Variable &args, Environment &env)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		a.requireType("remainder", Variable::NUMBER);
		b.requireType("remainder", Variable::NUMBER);
		return static_cast<number>(a) % static_cast<number>(b);
	}),

	PrimitiveProcdeure("not", [](const Variable &args, Environment &env)->Variable{
		Variable a = args.car();
		return BOOL_TO_VAR(a.isEqual("false"));
	}),

	PrimitiveProcdeure("cons", [](const Variable& args, Environment &env)->Variable{
		Variable a = args.car();
		Variable b = args.cdr().car();
		return Variable(a, b);
	}),

	PrimitiveProcdeure("car", [](const Variable &args, Environment &env)->Variable{
		Variable pair = args.car();
		return pair.car();
	}),

	PrimitiveProcdeure("cdr", [](const Variable &args, Environment &env)->Variable{
		Variable pair = args.car();
		return pair.cdr();
	}),

	PrimitiveProcdeure("set-car!", [](const Variable &args, Environment &env)->Variable{
		Variable pair = args.car();
		Variable car = args.cdr().car();
		return pair.setCar(car);
	}),

	PrimitiveProcdeure("set-cdr!", [](const Variable &args, Environment &env)->Variable{
		Variable pair = args.car();
		Variable cdr = args.cdr().car();
		return pair.setCdr(cdr);
	}),

	PrimitiveProcdeure("list", [](const Variable &args, Environment &env)->Variable{
		return args;
	}),

	PrimitiveProcdeure("length", [](const Variable &args, Environment &env)->Variable{
		int length = 0;
		for (Variable it = args.car(); !it.isNull(); it = it.cdr(), length++);
		return length;
	}),

	PrimitiveProcdeure("append", [](const Variable &args, Environment &env)->Variable{
		Variable head = Variable(VAR_NULL, VAR_NULL);
		Variable tail = head;
		for (Variable it = args; !it.isNull(); it = it.cdr()) {
			Variable li = it.car();
			if (!li.isNull()) {
				tail.setCdr(li);
				while (!tail.cdr().isNull())
					tail = tail.cdr();
			}
		}
		return head.cdr();
	}),

	PrimitiveProcdeure("map", [](const Variable &args, Environment &env)->Variable{
		Variable proc = args.car();
		// get arguments
		std::list<Variable> argss;
		for (Variable it = args.cdr(); !it.isNull(); it = it.cdr())
			argss.push_back(it.car());
		// apply
		Variable head = Variable(VAR_NULL, VAR_NULL);
		Variable tail = head;
		while (true) {
			Variable argshead = Variable(VAR_NULL, VAR_NULL);
			Variable argstail = argshead;
			bool stop = false;
			for (auto it = argss.begin(); it != argss.end(); it++)
				if (it->isNull()) {	// two few arguments
					stop = true;
					break;
				} else {			// success
					Variable narg = Variable(it->car(), VAR_NULL);
					argstail.setCdr(narg);
					argstail = narg;
					*it = it->cdr();
				}
			if (stop) {	// two few arguments
				break;
			} else {	// sccuess
				Variable ntail = Variable(apply(proc, argshead.cdr(), env), VAR_NULL);
				tail.setCdr(ntail);
				tail = ntail;
			}
		}
		return head.cdr();
	}),

	PrimitiveProcdeure("newline", [](const Variable &args, Environment &env)->Variable{
		std::cout << std::endl;
		return VAR_VOID;
	}),

	PrimitiveProcdeure("display", [](const Variable &args, Environment &env)->Variable{
		std::cout << args.car();
		return VAR_VOID;
	}),

	PrimitiveProcdeure("read", [](const Variable &args, Environment &env)->Variable{
		Variable val = VAR_NULL;
		if (std::cin >> val)
			return val;
		throw SchemeException("read: end of file");
	}),

	PrimitiveProcdeure("error", [](const Variable &args, Environment &env)->Variable{
		std::string msg;
		for (Variable it = args; !it.isNull(); it = it.cdr())
			msg += it.car().toString();
		throw SchemeException(msg);
	}),

	PrimitiveProcdeure("eval", [](const Variable &args, Environment &env)->Variable{
		return eval(args.car(), env);
	}),

	PrimitiveProcdeure("apply", [](const Variable &args, Environment &env)->Variable{
		return apply(args.car(), args.cdr().car(), env);
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
	if (IS_APPLICATION(expr))
		return apply(eval(APPLICATION_NAME(expr), env), evalArgs(APPLICATION_ARGS(expr), env), env);
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
		extendEnv.defineVariable(BINDING_VAR(binding), eval(BINDING_VAL(binding), env));
	}
	return evalSeq(LET_BODY(expr), extendEnv);
}

Variable evalArgs(const Variable &args, Environment &env)
{
	Variable head = Variable(VAR_NULL, VAR_NULL);
	Variable tail = head;
	for (Variable it = args; !it.isNull(); it = it.cdr()) {
		Variable ntail = Variable(eval(it.car(), env), VAR_NULL);
		tail.setCdr(ntail);
		tail = ntail;
	}
	return head.cdr();
}

Variable apply(const Variable &proc, const Variable &vals, Environment &env)
{
	if (proc.isPrim())
		// eval primitives
		try {
			return proc(vals, env);
		} catch (SchemeException e) {
			e.addTrace(proc.toString());
			throw e;
		}
	if (proc.isComp()) {
		CompoundProcedure comp = CompoundProcedure(proc);
		Variable body = comp.getSequence();
		Variable args = comp.getArguments();
		Environment extendEnv = Environment(args, vals, comp.getEnvironmrnt());
		// eval compound
		try {
			return evalSeq(body, extendEnv);
		} catch (SchemeException e) {
			e.addTrace(proc.toString() + ":" + body.toString());
			throw e;
		}
	}
	throw SchemeException(std::string("apply: can't apply ") + proc.toString());
}

Environment setupEnvironment()
{
	Environment env = Environment();
	// add constant 
	env.defineVariable(std::string("true"), VAR_TRUE);
	env.defineVariable(std::string("false"), VAR_FALSE);
	env.defineVariable(std::string("null"), VAR_NULL);
	// add primitive procedure
	for (const PrimitiveProcdeure &prim : prims)
		env.defineVariable(prim.getName(), prim);
	return env;
}

EVAL_NAMESPACE_END