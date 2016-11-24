%top {
#include <string>
#include "variable.hpp"
#include "parser.hpp"
}

line_comment	\;[^\r\n]*(\r|\n)
block_comment	\#\|([^\|]|\|[^\#])*\|\#
comment 		{line_comment}|{block_comment}
divider 		(\r|\n|" "|{comment})+
digit			[[:digit:]]
signed			-?
rational		{signed}{digit}+(\/{digit}+)?
double			{signed}{digit}+(\.{digit}+)?((e|E){signed}{digit}+)?
symbol			[^\"\(\)\.\r\n" "]+
string 			\"(\\\"|[^\"])*\"

%%

\(					return LEFT_PARENTHESES;
\)					return RIGHT_PARENTHESES;
'					return QUOTE;
\.					return DOT;
{string}		{ 
	yylval = Variable(std::string(yytext+1, yytext+yyleng-1), Variable::TYPE_STRING); 
	return STRING; 
}
{rational}		{
	yylval = Variable(std::string(yytext), Variable::TYPE_RATIONAL);
	return RATIONAL;
}
{double}		{
	yylval = Variable(std::string(yytext), Variable::TYPE_DOUBLE);
	return DOUBLE;
}
{symbol}		{
	yylval = Variable(std::string(yytext), Variable::TYPE_SYMBOL);
	return SYMBOL;
}
{divider}		return DIVIDER;

%%