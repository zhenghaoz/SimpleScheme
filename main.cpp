#include <iostream>
#include <cctype>
#include <vector>
#include "variable.h"
#include "exception.h"
#include "evaluator.h"
#include "parser.h"

using namespace std;
using namespace eva;

#define EVAL(exp)	do {\
	Variable val = eval(parse(exp), env);\
	if (!val.isVoid())\
		cout << val << endl;\
} while (0)

int main(int argc, char const *argv[])
{
	try {
		Environment env = setupEnvironment();
		EVAL("1");
		EVAL("\"Hello World\"");
		EVAL("(define a 1)");
		EVAL("a");
		EVAL("(set! a 64)");
		EVAL("a");
		EVAL("(+ a 1)");
		EVAL("(- a 4)");
		EVAL("(/ a 4)");
		EVAL("(define (sgn x) (cond ((> x 0) 1) ((< x 0) -1) (else 0)))");
		EVAL("(sgn 12)");
		EVAL("(sgn 0)");
		EVAL("(sgn -12)");
		EVAL("(define (make-counter) (let ((count 10)) (lambda (msg) (cond ((eq? msg (quote inc)) (set! count (+ count 1)) count) ((eq? msg (quote dec)) (set! count (- count 1)) count)))))");
		EVAL("(define counter (make-counter))");
		EVAL("(counter (quote inc))");
		EVAL("(counter (quote inc))");
		EVAL("(counter (quote dec))");
		EVAL("(eval (quote (+ 1 1)))");
		EVAL("(define (foo x) (if (= x 0) 1 (* x (foo (- x 1)))))");
		EVAL("(foo 5)");
		EVAL("(apply + (quote (1 2 3 4 5)))");
		EVAL("(define p (cons 1 2))");
		EVAL("p");
		EVAL("(car p)");
		EVAL("(cdr p)");
		EVAL("(set-car! p 10)");
		EVAL("(set-cdr! p 12)");
		EVAL("p");
/*		Variable var("+", [](const Variable &args) -> Variable {
			Variable sum = car(args);
			for (Variable rest = cdr(args); rest.isNull() ; rest = cdr(rest))
				sum = sum + car(rest);
			return sum;
		});
		cout << var << endl;
		cout << var(parse("(1 2 3 4 5)")) << endl;*/
	} catch (SchemeException e) {
		cout << e.what() << endl;
	}
	return 0;
}