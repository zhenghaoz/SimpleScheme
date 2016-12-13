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

	void printStatistic()
	{
		clog << "Variale created   " << varCreated << endl;
		clog << "Variale copyed    " << varCopyed << endl;
		clog << "Variale destroyed " << varDestroyed << endl;
		clog << "Variale alive     " << varCreated - varDestroyed << endl;
		clog << "Variale traced    " << varTraced << endl;
	}

}