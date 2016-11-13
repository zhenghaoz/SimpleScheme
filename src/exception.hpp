// 
// Exception for Scheme
// 
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
// 
#pragma once

#include <string>
#include <vector>

class Exception
{
	// Type alias
	using string = std::string;
	using vector = std::vector<string>;

	// Data member
	string msg;
	vector trace;

public:

	// Constructor
	Exception(const string& msg): msg(msg) {}

	void addTrace(const string& exp) 
	{ 
		trace.push_back(exp);
	}

	void printStack()
	{
		using std::cout;
		using std::endl;

		cout << msg << endl;
		for (int i = 0; i < trace.size(); i++)
			if (i == 0)
				cout << "\tfrom: " << trace[i] << endl;
			else
				cout << "\t      " << trace[i] << endl;
	}
};