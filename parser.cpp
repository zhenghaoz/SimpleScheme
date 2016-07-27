#include <string>
#include <stack>
#include "parser.h"
#include "exception.h"

EVAL_NAMESPACE_BEGIN

bool isNumber(const std::string &str);
Variable parse(const std::string &str, int &pos);
Variable parseString(const std::string &str, int &pos);
Variable parseList(const std::string &str, int &pos);
Variable parseWord(const std::string &str, int &pos);

Variable parse(const std::string &str)
{
	int pos = 0;
	return parse(str, pos);
}

Variable parse(const std::string &str, int &pos)
{
	/* ignore space */
	if (str[pos] == ' ')
		pos++;
	/* dispatch */
	int len = str.length();
	switch (str[pos]) {
		case '(':
			return parseList(str, pos);
		case '"':
			return parseString(str, pos);
		default:
			return parseWord(str, pos);
	}
}

bool isNumber(const std::string &str)
{
	int i = 0;
	bool sgn = false;
	if (str[i] == '-') {
		sgn = true;
		i++;
	}
	for (; str[i]; i++)
		if (!isdigit(str[i]))
			return false;
	return !sgn || i > 1;
}

Variable parseString(const std::string &str, int &pos)
{
	pos++;
	int start = pos;
	while (str[pos] && str[pos] != '"')
		pos++;
	if (str[pos] == '\0')
		throw SchemeException("parser: expect \"\n\tline: " + str + "\n\tposition: " + std::to_string(start-1) + "\n");
	return Variable(str.substr(start, pos-start), Variable::STRING);
}

Variable parseList(const std::string &str, int &pos)
{
	std::stack<Variable> varStack;
	int start = pos;
	pos++;
	while (str[pos] && str[pos] != ')') {
		while (str[pos] && str[pos] == ' ')
			pos++;
		varStack.push(parse(str, pos));
	}
	if (str[pos] == '\0')
		throw SchemeException("parser: expect )\n\tline: " + str + "\n\tposition: " + std::to_string(start) + "\n");
	pos++;
	Variable list = VAR_NULL;
	while (!varStack.empty()) {
		list = Variable(varStack.top(), list);
		varStack.pop();
	}
	return list;
}

Variable parseWord(const std::string &str, int &pos)
{
	int start = pos;
	while (str[pos] && str[pos] != ' ' && str[pos] != ')')
		pos++;
	int len = pos - start;
	if (len == 0)
		return VAR_NULL;
	std::string word = str.substr(start, len);
	return isNumber(word) ? Variable(std::stoi(word)) : Variable(word);
}

EVAL_NAMESPACE_END