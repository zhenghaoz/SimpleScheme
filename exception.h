#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <vector>

EVAL_NAMESPACE_BEGIN

class SchemeException
{
	std::string _msg;
	std::vector<std::string> _trace;
public:
	SchemeException(std::string msg): _msg(msg) {}
	void addTrace(const std::string &exp) { _trace.push_back(exp); }
	void printStack()
	{
		std::cout << _msg << std::endl;
		int length = _trace.size();
		for (int i = 0; i < length; i++)
			if (i == 0)
				std::cout << "\tfrom: " << _trace[i] << std::endl;
			else
				std::cout << "\t      " << _trace[i] << std::endl;
	}
};

EVAL_NAMESPACE_END

#endif