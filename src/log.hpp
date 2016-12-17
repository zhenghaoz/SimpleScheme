// 
// Scheme logger
// 
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#pragma once

#include <iostream>

#define VERBOSE(tag,msg)	(std::clog<<"\x1B[1;36m"<<tag<<':'<<msg<<"\x1B[0m"<<std::endl)
#define ERROR(tag,msg)		(std::clog<<"\x1B[1;35m"<<tag<<':'<<msg<<"\x1B[0m"<<std::endl)
#define DEBUG(tag,msg)		(std::clog<<"\x1B[1;34m"<<tag<<':'<<msg<<"\x1B[0m"<<std::endl)