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

int main(int argc, char const *argv[])
{
	Environment env = setupEnvironment();
	while (true) {
		try {
			cout << "> ";
			Variable exp;
			if (!(cin >> exp))
				return EOF;
			Variable val = eval(exp, env);
			if (!val.isVoid())
				cout << val << endl;
		} catch (SchemeException e) {
			cout << e.what() << endl;
		}
		GarbageCollector::collect();
	}
	return 0;
}