%code top {
#include <iostream>
#include <FlexLexer.h>
#include "variable.hpp"
#include "parser.hpp"

yyFlexLexer lexer;			// Lexer

int yylex();				// yylex wrapper
void yyerror(char const *);
Variable yypval = VAR_NULL;
}

%define api.value.type {Variable}

%token LEFT_PARENTHESES
%token RIGHT_PARENTHESES
%token QUOTE
%token DOT
%token STRING
%token RATIONAL
%token DOUBLE
%token SYMBOL
%token DIVIDER
%token END_OF_FILE

%%

input:
  %empty				{ return EOF;	}
| END_OF_FILE 			{ return EOF;	}
| DIVIDER END_OF_FILE 	{ return EOF;	}
| exp					{ yypval = $1; return 0;	}
| DIVIDER exp			{ yypval = $2; return 0;	}
;

exp:
  RATIONAL 											{ $$ = $1;	}
| DOUBLE 											{ $$ = $1;	}
| SYMBOL 											{ $$ = $1;	}
| STRING 											{ $$ = $1;	}
| LEFT_PARENTHESES seq RIGHT_PARENTHESES			{ $$ = $2;	}
| LEFT_PARENTHESES DIVIDER seq RIGHT_PARENTHESES	{ $$ = $3;	}
| QUOTE exp											{ 
	$$ = Variable(Variable("quote", Variable::TYPE_SYMBOL),Variable($2,VAR_NULL));	
}
| QUOTE DIVIDER exp									{ 
	$$ = Variable(Variable("quote", Variable::TYPE_SYMBOL),Variable($3,VAR_NULL));	
}
;

seq:
  %empty						{ $$ = VAR_NULL;				}
| exp seq						{ $$ = Variable($1,$2);			}
| exp DIVIDER seq				{ $$ = Variable($1,$3);			}
| exp DIVIDER DOT DIVIDER exp	{ $$ = Variable($1,$5);			}

%%

void yyerror(char const *s)
{
	fprintf(stderr, "%s\n", s);
}

int yylex()
{ 
	return lexer.yylex();
}

int yyparse(std::istream* in, std::ostream* out = 0)
{
	lexer.switch_streams(in, out);
	return yyparse();
}