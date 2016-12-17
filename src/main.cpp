#include <iostream>
#include <fstream>
#include "variable.hpp"
#include "primitive.hpp"
#include "evaluator.hpp"
#include "exception.hpp"
#include "statistic.hpp"

using namespace std;

int evaluator(istream& in, const string prompt = "")
{
	int errorcnt = 0;
	Variable var;
	// Setup initial environment
	Environment env = Primitive::setupEnvironment();
	while (cout << prompt && in >> var) {
		try {
			Variable ret = Evaluator::eval(var, env);
			if (ret != VAR_VOID)
				cout << ret << endl;
		} catch (Exception e) {
			e.printStack();
			errorcnt++;
		}
		// Collect garbage
		GarbageCollector::collect(env);
		// Print statistic information
		#ifdef STATS
		Statistic::printStatistic();
		#endif
	}
	return errorcnt;
}

int main(int argc, char const *argv[])
{
	if (argc > 1) {	// Read from file
		ifstream fin(argv[1]);
		return evaluator(fin);		
	} else {		// Read from cin
		cout << "Welcome to Simple Scheme v0.1" << endl;
		return evaluator(cin, ">");
	}
	return 0;
}
