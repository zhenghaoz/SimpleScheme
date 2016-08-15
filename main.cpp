#include <iostream>
#include <cctype>
#include <fstream>
#include <vector>
#include "variable.h"
#include "exception.h"
#include "evaluator.h"
#include "parser.h"

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
	if (argc > 1)
		EVAL_LOOP(fstream(argv[1]));
	else
		EVAL_LOOP(cin);	
	return 0;
}