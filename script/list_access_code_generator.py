# 
# List access primtive procedure code generator
# 
# Author: ZhangZhenghao (zhangzhenghao@hotmail.com)
# 

def generator(n, text = ''):
	if (len(text) > 0):
		code = 'Variable("c' + text + 'r", [](const Variable& args, Environment& env)->Variable{\n'
		code += '\treturn FIRST_ARG(args)'
		for i in text:
			code += '.c' + i + 'r()'
		code += ';\n'
		code += '}),\n';
		print(code)
	if (n > 0):
		generator(n-1, text+'a')
		generator(n-1, text+'d')

generator(4)