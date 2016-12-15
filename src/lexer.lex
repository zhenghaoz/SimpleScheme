%top {
// 
// Scheme lexer
// 
// Author: Zhang Zhenghao (zhangzhenghao@hotmail.com)
//
#include <string>
#include "variable.hpp"
#include "parser.hpp"
}

%option noyywrap
%option c++

line_comment	\;[^\r\n]*(\r|\n)
block_comment	\#\|([^\|]|\|[^\#])*\|\#
comment 		{line_comment}|{block_comment}
divider 		(\r|\n|\t|" "|{comment})+
digit			[[:digit:]]
signed			-?
rational		{signed}{digit}+(\/{digit}+)?
double			{signed}{digit}+(\.{digit}+)?((e|E){signed}{digit}+)?
symbol			[^'\"\(\)\.\r\n" "]+
string 			\"(\\\"|[^\"])*\"

%%

<<EOF>>			return END_OF_FILE;
\(				return LEFT_PARENTHESES;
\)				return RIGHT_PARENTHESES;
'				return QUOTE;
\.				return DOT;
{string}		{ 
	yylval = Variable(std::string(YYText()+1, YYText()+YYLeng()-1), Variable::TYPE_STRING); 
	return STRING; 
}
{rational}		{
	yylval = Variable(YYText(), Variable::TYPE_RATIONAL);
	return RATIONAL;
}
{double}		{
	yylval = Variable(YYText(), Variable::TYPE_FLOAT);
	return DOUBLE;
}
{symbol}		{
	yylval = Variable::createSymbol(YYText());
	return SYMBOL;
}
{divider}		return DIVIDER;

%%
