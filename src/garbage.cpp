// 
// Garbage collector
// 
// Author: ZhangZhenghao (zhangzhenghao@hotmail.com)
// 
#include <vector>
#include <cstdlib>
#include "garbage.hpp"
#include "variable.hpp"

#ifdef STATS
#include "statistic.hpp"
#endif

using std::vector;

namespace {
	vector<Variable> traceList;
}

namespace GarbageCollector {

	void trace(const Variable& var)
	{
		traceList.push_back(var);
		#ifdef STATS
		Statistic::traceVariable();
		#endif
	}

	void collect(Environment& env)
	{
		int tag = rand();
		env.scan(tag);
		vector<Variable> aliveList;
		for (const Variable& var : traceList)
			if (*var.gcTag == tag) {
				aliveList.push_back(var);
			} else {
				var.finalize();
				#ifdef STATS
				Statistic::finalizeVariable();
				#endif
			}
		std::swap(aliveList, traceList);
	}
}