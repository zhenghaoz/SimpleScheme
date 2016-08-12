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
		if (!_trace.empty()) {
			std::cout << "from" << std::endl;
			for (const std::string &trace : _trace)
				std::cout << "\t" << trace << std::endl;
		}
	}
};

EVAL_NAMESPACE_END

#endif