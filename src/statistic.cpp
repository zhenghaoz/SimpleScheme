// 
// Statistic unit
// 
// Author: ZhangZhenghao(zhangzhenghao@hotmail.com)
// 
#include "statistic.hpp"

using namespace std;

namespace {

	int varCreated = 0;
	int varDestroyed = 0;
	int varCopyed = 0;
	int varTraced = 0;
	int stackDepth = 0;
	int stackMaxDepth = 0;

}

namespace Statistic {

	void createVariable()
	{
		varCreated++;
	}

	void destroyVariable()
	{
		varDestroyed++;
	}

	void copyVariable()
	{
		varCopyed++;
	}

	void traceVariable()
	{
		varTraced++;
	}

	void finalizeVariable()
	{
		varTraced--;
	}

	void applyStart()
	{
		stackDepth++;
		if (stackDepth > stackMaxDepth)
			stackMaxDepth = stackDepth;
	}

	void applyEnd()
	{
		stackDepth--;
	}

	void printStatistic()
	{
		clog << "\x1B[1;33mvariale created   " << varCreated << endl;
		clog << "variale copyed    " << varCopyed << endl;
		clog << "variale destroyed " << varDestroyed << endl;
		clog << "variale alive     " << varCreated - varDestroyed << endl;
		clog << "variale traced    " << varTraced << endl;
		clog << "max stack depth   " << stackMaxDepth << "\x1B[0m" << endl;
	}

}