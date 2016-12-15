#include <iostream>
#include "variable.hpp"
#include "primitive.hpp"
#include "evaluator.hpp"
#include "exception.hpp"
#include "statistic.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
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
