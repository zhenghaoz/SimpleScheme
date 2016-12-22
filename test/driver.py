#!/usr/bin/python3

# 
# Test driver
# 
# Author: ZhangZhenghao (zhangzhenghao@hotmail.com)
# 
import os
import time

# Config
EXECUTE		= '../bin/main'
PATH		= os.path.dirname(os.path.realpath(__file__))

start_time_total = time.time()
total = 0
accepted = 0
files = os.listdir(PATH)
for file in files:
	if file.endswith('.scm'):
		total += 1
		start_time = time.time()
		result = os.system(EXECUTE + ' < ' + PATH + '/' + file + ' > /dev/null')
		if result == 0:
			accepted += 1
			print('accepted', end='')
		else:
			print('error(' + str(result>>8) +')', end='')
		end_time = time.time()
		print('\t{:.3f}s\t{:s}'.format(end_time - start_time, file))
end_time_total = time.time();
print('{:d}/{:d} passed\t{:.3f}s'.format(accepted, total, end_time_total - start_time_total))
