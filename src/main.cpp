#include <iostream>
#include <fstream>
#include "variable.hpp"
#include "primitive.hpp"
#include "evaluator.hpp"
#include "exception.hpp"
#include "statistic.hpp"

using namespace std;

int console()
{
	cout << "Welcome to Simple Scheme v0.1" << endl;
	Variable var;
	// Setup initial environment
	Environment env = Primitive::setupEnvironment();
	while (cout << '>' && cin >> var) {
		try {
			Variable ret = Evaluator::eval(var, env);
			if (ret != VAR_VOID)
				cout << ret << endl;
		} catch (Exception e) {
			e.printStack();
		}
		// Collect garbage
		GarbageCollector::collect(env);
		// Print statistic information
		#ifdef STATS
		Statistic::printStatistic();
		#endif
	}
	return 0;
}

int file(istream& in)
{
	Variable var;
	// Setup initial environment
	Environment env = Primitive::setupEnvironment();
	while (in >> var) {
		try {
			Variable ret = Evaluator::eval(var, env);
			if (ret != VAR_VOID)
				cout << ret << endl;
		} catch (Exception e) {
			e.printStack();
		}
		// Collect garbage
		GarbageCollector::collect(env);
		// Print statistic information
		#ifdef STATS
		Statistic::printStatistic();
		#endif
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	if (argc > 1) {
		ifstream fin(argv[1]);
		file(fin);
	} else {
		console();
	}
	return 0;
}
