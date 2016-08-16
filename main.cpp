#include <iostream>
#include <fstream>
#include "variable.hpp"
#include "exception.hpp"
#include "evaluator.hpp"
#include "parser.hpp"
#include "primitive.hpp"

using namespace std;
using namespace eva;

#define EVAL_LOOP(in) do {\
	Environment env = setupEnvironment();\
	GarbageCollector::setGlobalEnvironment(env);\
	while (true) {\
		try {\
			Variable exp;\
			if (!(in >> exp))\
				break;\
			Variable val = eval(exp, env);\
			if (!val.isVoid())\
				cout << val << endl;\
		} catch (SchemeException e) {\
			e.printStack();\
		}\
		GarbageCollector::collect();\
	}\
} while (0)

int main(int argc, char const *argv[])
{
	if (argc > 1) {
		ifstream fin(argv[1]);
		EVAL_LOOP(fin);
	} else
		EVAL_LOOP(cin);	
	return 0;
}