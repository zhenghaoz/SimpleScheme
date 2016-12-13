// 
// Garbage collector
// 
// Author: ZhangZhenghao (zhangzhenghao@hotmail.com)
// 
#pragma once

#include <memory>

class Environment;
class Variable;

namespace GarbageCollector {

	void trace(const Variable& var);
	void collect(Environment& env);
	
}

class GarbageObject
{
protected:

	friend void GarbageCollector::collect(Environment& env);

	// Tag for GC
	std::shared_ptr<int> gcTag;

public:

	GarbageObject(): gcTag(std::make_shared<int>(0)) {}

	// Finalize value
	virtual void finalize() const {};

	// Scan and tag value in using
	virtual void scan(int tag) const {};
};