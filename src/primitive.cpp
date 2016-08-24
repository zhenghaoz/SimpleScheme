#include <list>
#include "evaluator.hpp"
#include "parser.hpp"
#include "primitive.hpp"

#define BOOL_TO_VAR(exp)		(exp ? VAR_TRUE : VAR_FALSE)

EVAL_NAMESPACE_BEGIN

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

Environment setupEnvironment()
{
	Environment env;
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