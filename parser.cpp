#include <string>
#include <stack>
#include "parser.h"
#include "exception.h"

EVAL_NAMESPACE_BEGIN

/* declare */

bool isNumber(const std::string &str);
Variable parse(istream &in);
Variable parseString(istream &in);
Variable parseList(istream &in);
Variable parseWord(istream &in);

/* variable stream */

istream &operator>>(istream &in, Variable &var)
{
	var = parse(in);
	return in;
}

/* parsers */

// dispatcher
Variable parse(istream &in)
{
	int ch = 0;
	/* ignore space */
	while (static_cast<char>(ch = in.peek()) == ' '
		|| static_cast<char>(ch) == '\n')
		in.get();
	/* dispatch */
	switch (char(ch)) {
		case '(':
			return parseList(in);
		case '"':
			return parseString(in);
		case '\'':
			in.get();
			return Variable(Variable("quote"), Variable(parse(in), VAR_NULL));
		default:
			return parseWord(in);
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

// parse string
Variable parseString(istream &in)
{
	int ch = 0;
	in.get();
	string str;
	while ((ch = in.peek()) != EOF && static_cast<char>(ch) != '"')
		str.push_back(static_cast<char>(in.get()));
	if (ch == EOF)
		throw SchemeException("parser: expect \"");
	return Variable(str, Variable::STRING);
}

// parse list
Variable parseList(istream &in)
{
	// get list items
	std::stack<Variable> varStack;
	int ch;
	in.get();
	while ((ch = in.peek()) != EOF && static_cast<char>(ch) != ')')
		varStack.push(parse(in));
	if (ch == EOF)
		throw SchemeException("parser: expect )");
	in.get();
	// construct linked list
	Variable list = VAR_NULL;
	while (!varStack.empty()) {
		list = Variable(varStack.top(), list);
		varStack.pop();
	}
	return list;
}

// parse symbol and number
Variable parseWord(istream &in)
{
	string word;
	int ch = 0;
	while ((ch = in.peek()) != EOF 
		&& static_cast<char>(ch) != ' ' 
		&& static_cast<char>(ch) != '\n'
		&& static_cast<char>(ch) != ')')
		word.push_back(static_cast<char>(in.get()));
	if (word.empty())
		return VAR_NULL;
	return isNumber(word) ? Variable(std::stoi(word)) : Variable(word);
}

EVAL_NAMESPACE_END