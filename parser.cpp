#include <map>
#include <string>
#include "parser.hpp"
#include "exception.hpp"

#define IS_NEWLINE(ch) (ch == '\r' || ch == '\n')
#define IS_SPACE(ch) (ch == ' ' || ch == '\t' || IS_NEWLINE(ch))
#define IS_WORD_BOUND(ch) (IS_SPACE(ch) || ch == ';' || ch == ')')

#define REMOVE_SEMI_COMMENT(in) do {\
	int ch = 0;\
	while ((ch = in.peek()) != EOF && !IS_NEWLINE(static_cast<char>(ch)))\
		in.get();\
} while (0)

#define REMOVE_SHARP_COMMENT(in) do {\
	while (!in.eof() && !endOfSharpComment(in));\
} while (0)

#define REMOVE_SPACE(in) do {\
	int ch = 0;\
	while ((ch = in.peek()) != EOF) {\
		if (IS_SPACE(static_cast<char>(ch)))\
			in.get();\
		else if (static_cast<char>(ch) == ';')\
			REMOVE_SEMI_COMMENT(in);\
		else if (startOfSharpComment(in))\
			REMOVE_SHARP_COMMENT(in);\
		else break;\
	}\
} while (0)

EVAL_NAMESPACE_BEGIN

using istream = std::istream;
using string = std::string;

/* declare */

bool isNumber(const string &str);
Variable parse(istream &in);
Variable parseString(istream &in);
Variable parseList(istream &in);
Variable parseWord(istream &in);
void removeQuoteComment(istream &in);
bool startOfSharpComment(istream &in);
bool endOfSharpComment(istream &in);

/* symbol pool */

static map<string, Variable> _symbolPool;

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
	/* remove space */
	REMOVE_SPACE(in);
	/* dispatch */
	switch (static_cast<char>(in.peek())) {
		case ')':
			in.get();
			throw SchemeException("parser: expect (");
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

bool isNumber(const string &str)
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
	in.get();	// remove left "
	string str;
	while ((ch = in.peek()) != EOF && static_cast<char>(ch) != '"')
		str.push_back(static_cast<char>(in.get()));
	if (ch == EOF)
		throw SchemeException("parser: expect \"");
	in.get();	// remove right "
	return Variable(str, Variable::STRING);
}

// parse list
Variable parseList(istream &in)
{
	Variable head = Variable(VAR_NULL, VAR_NULL);
	Variable tail = head;
	int ch;
	in.get();	// remove (
	REMOVE_SPACE(in);
	while ((ch = in.peek()) != EOF && static_cast<char>(ch) != ')') {
		// add item to list
		Variable ntail = Variable(parse(in), VAR_NULL);
		tail.setCdr(ntail);
		tail = ntail;
		REMOVE_SPACE(in);
	}
	if (ch == EOF)
		throw SchemeException("parser: expect )");
	in.get();	// remove )
	return head.cdr();
}

// parse symbol and number
Variable parseWord(istream &in)
{
	string word;
	int ch = 0;
	while ((ch = in.peek()) != EOF && !IS_WORD_BOUND(static_cast<char>(ch)))
		word.push_back(static_cast<char>(in.get()));
	if (word.empty())
		return VAR_NULL;
	// number
	if (isNumber(word))
		return Variable(std::stoi(word));
	// symbol
	if ( _symbolPool.find(word) == _symbolPool.end())
		_symbolPool[word] = Variable(word);
	return _symbolPool[word];
}

// comment

bool startOfSharpComment(istream &in)
{
	int ch = in.peek();
	if (static_cast<char>(ch) != '#')
		return false;
	in.get();		// remove #
	if (static_cast<char>(ch = in.peek()) == '|') {
		in.get();	// remove |
		return true;
	}
	in.putback('#');// putback #
	return false;
}

bool endOfSharpComment(istream &in)
{
	int ch = in.get();
	if (static_cast<char>(ch) != '|')
		return false;
	if (static_cast<char>(ch = in.peek()) == '#') {
		in.get();
		return true;
	}
	return false;
}

EVAL_NAMESPACE_END