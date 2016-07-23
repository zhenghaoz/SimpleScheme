#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

EVAL_NAMESPACE_BEGIN

class SchemeException
{
	std::string _msg;
public:
	SchemeException(std::string msg): _msg(msg) {}
	std::string what() { return _msg; };
};

EVAL_NAMESPACE_END

#endif