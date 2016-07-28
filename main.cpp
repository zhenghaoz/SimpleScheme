#include <iostream>
#include <cctype>
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
	Variable exp;
	while (cin >> exp)
		try {
			Variable val = eval(exp, env);
			if (!val.isVoid())
				cout << val << endl;
		} catch (SchemeException e) {
			cout << e.what() << endl;
		}
	return 0;
}