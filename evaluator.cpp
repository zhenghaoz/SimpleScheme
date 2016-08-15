#include <string>
#include "evaluator.h"
#include "variable.h"
#include "exception.hpp"

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

EVAL_NAMESPACE_BEGIN

using string = std::string;

Variable evalAnd(const Variable &expr, Environment &env);
Variable evalOr(const Variable &expr, Environment &env);
Variable evalSeq(const Variable &expr, Environment &env);
Variable evalCond(const Variable &expr, Environment &env);
Variable evalLet(const Variable &expr, Environment &env);
Variable evalArgs(const Variable &args, Environment &env);

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
	throw SchemeException(string("eval: can't evaluate ") + expr.toString());
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
	throw SchemeException(string("apply: can't apply ") + proc.toString());
}

EVAL_NAMESPACE_END