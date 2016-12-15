// 
// Scheme Evaluator
// 
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include "evaluator.hpp"
#include "variable.hpp"
#include "exception.hpp"

// Macro for evaluating

#define TAGGED_LIST(exp, tag)	((exp).isPair() && (exp).car().toString() == (tag))
// BOOL
#define IS_TRUE(exp)			((exp) != VAR_FALSE)
#define IS_FALSE(exp)			((exp) == VAR_FALSE)
// SELF EVALUATING
#define IS_SELF_EVALUATING(exp)	((exp).isNumber() || (exp).isString())
// VARIABLE
#define IS_VARIABLE(exp)		((exp).isSymbol())
// QUOTED
#define IS_QUOTED(exp)			TAGGED_LIST(exp, "quote")
#define QUOTED(exp)				((exp).cdr().car())
// DEFINE
#define IS_DEFINE(exp)			TAGGED_LIST(exp, "define")
#define IS_DEFINE_VAR(exp)		(IS_DEFINE(exp) && exp.cdr().car().isSymbol())
#define IS_DEFINE_PROC(exp)		(IS_DEFINE(exp) && exp.cdr().car().isPair())
#define DEFINE_VAR_NAME(exp)	((exp).cdr().car())
#define DEFINE_VAR_VAL(exp)		((exp).cdr().cdr().car())
#define DEFINE_PROC_NAME(exp)	((exp).cdr().car().car())
#define DEFINE_PROC_ARGS(exp)	((exp).cdr().car().cdr())
#define DEFINE_PROC_BODY(exp)	((exp).cdr().cdr())
// ASSIGNMENT
#define IS_ASSIGNMENT(exp)		TAGGED_LIST(exp, "set!")
#define ASSIGNMENT_VAR(exp)		((exp).cdr().car())
#define ASSIGNMENT_VAL(exp)		((exp).cdr().cdr().car())
// SEQUENCE
#define IS_SEQ(exp)				TAGGED_LIST(exp, "begin")
#define SEQUENCE(exp)			((exp).cdr())
// LAMBDA
#define IS_LAMBDA(exp)			TAGGED_LIST(exp, "lambda")
#define LAMBDA_ARGS(exp)		((exp).cdr().car())
#define LAMBDA_BODY(exp)		((exp).cdr().cdr())
// AND
#define IS_AND(exp)				TAGGED_LIST(exp, "and")
#define AND_ARGS(exp)			((exp).cdr())
// OR
#define IS_OR(exp)				TAGGED_LIST(exp, "or")
#define OR_ARGS(exp)			((exp).cdr())
// IF
#define IS_IF(exp)				TAGGED_LIST(exp, "if")
#define IF_PRED(exp)			((exp).cdr().car())
#define IF_CON(exp)				((exp).cdr().cdr().car())
#define IF_ALTER(exp)			((exp).cdr().cdr().cdr().car())
#define IS_APPLICATION(exp)		((exp).isPair())
#define IS_COND(exp)			TAGGED_LIST(exp, "cond")
#define IS_ELSE(exp)			TAGGED_LIST(exp, "else")
// COND
#define COND_CLUASES(exp)		((exp).cdr())
#define COND_PRED(exp)			((exp).car())
#define COND_CONSEQUENCE(exp)	((exp).cdr())
// LET
#define IS_LET(exp)				TAGGED_LIST(exp, "let")
#define LET_BINDINGS(exp)		((exp).cdr().car())
#define BINDING_VAR(exp)		((exp).car())
#define BINDING_VAL(exp)		((exp).cdr().car())
#define LET_BODY(exp)			((exp).cdr().cdr())
// APPLICATION
#define APPLICATION_NAME(exp)	((exp).car())
#define APPLICATION_ARGS(exp)	((exp).cdr())

using namespace std;
using namespace Evaluator;

namespace {

	// Evaluate and
	Variable evalAnd(const Variable &expr, Environment &env)
	{
		for (Variable it = AND_ARGS(expr); it != VAR_NULL; it = it.cdr())
			if (IS_FALSE(eval(it.car(), env)))
				return VAR_FALSE;
		return VAR_TRUE;
	}

	// Evaluate or
	Variable evalOr(const Variable &expr, Environment &env)
	{
		for (Variable it = OR_ARGS(expr); it != VAR_NULL; it = it.cdr())
			if (IS_TRUE(eval(it.car(), env)))
				return VAR_TRUE;
		return VAR_FALSE;
	}

	// Evaluate sequence
	Variable evalSeq(const Variable &expr, Environment &env)
	{
		Variable val = VAR_VOID;
		for (Variable it = expr; it != VAR_NULL; it = it.cdr())
			val = eval(it.car(), env);
		return val;
	}

	// Evaluate condition
	Variable evalCond(const Variable &expr, Environment &env)
	{
		for (Variable clauses = COND_CLUASES(expr); clauses != VAR_NULL; clauses = clauses.cdr()) {
			Variable clause = clauses.car();
			if (IS_ELSE(clause) || IS_TRUE(eval(COND_PRED(clause), env)))
				return evalSeq(COND_CONSEQUENCE(clause), env);
		}
		return VAR_VOID;
	}

	// Evaluate let
	Variable evalLet(const Variable &expr, Environment &env)
	{
		Environment extendEnv = Environment(VAR_NULL, VAR_NULL, env);
		for (Variable bindings = LET_BINDINGS(expr); bindings != VAR_NULL; bindings = bindings.cdr()) {
			Variable binding = bindings.car();
			extendEnv.defineVariable(BINDING_VAR(binding), eval(BINDING_VAL(binding), env));
		}
		return evalSeq(LET_BODY(expr), extendEnv);
	}

	// Evaluate arguments
	Variable evalArgs(const Variable &args, Environment &env)
	{
		Variable head = Variable(VAR_NULL, VAR_NULL);
		Variable tail = head;
		for (Variable it = args; it != VAR_NULL; it = it.cdr()) {
			Variable ntail = Variable(eval(it.car(), env), VAR_NULL);
			tail.setCdr(ntail);
			tail = ntail;
		}
		return head.cdr();
	}

}

namespace Evaluator {

	// Evaluate dispatcher
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
					Variable(DEFINE_PROC_NAME(expr).toString(),
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
			return IS_TRUE(eval(IF_PRED(expr), env)) ? eval(IF_CON(expr), env) : eval(IF_ALTER(expr), env);
		if (IS_COND(expr))
			return evalCond(expr, env);
		if (IS_LAMBDA(expr))
			return Variable("lambda expression", LAMBDA_ARGS(expr), LAMBDA_BODY(expr), env);
		if (IS_LET(expr))
			return evalLet(expr, env);
		if (IS_APPLICATION(expr))
			return apply(eval(APPLICATION_NAME(expr), env), evalArgs(APPLICATION_ARGS(expr), env), env);
		throw Exception(string("eval: can't evaluate ") + expr.toString());
	}

	// Apply procedure
	Variable apply(const Variable &proc, const Variable &vals, Environment &env)
	{
		// Apply primitives
		if (proc.isPrim()) {
			try {
				return proc(vals, env);
			} catch (Exception e) {
				e.addTrace(proc.toString());
				throw e;
			}
		}
		// Apply compound
		if (proc.isComp()) {
			Variable body = proc.getProcedureBody();
			Variable args = proc.getProcedureArgs();
			Environment extendEnv = Environment(args, vals, proc.getProcedureEnv());
			try {
				return evalSeq(body, extendEnv);
			} catch (Exception e) {
				e.addTrace(proc.toString() + ":" + body.toString());
				throw e;
			}
		}
		// Exception
		throw Exception(string("apply: can't apply ") + proc.toString());
	}

}