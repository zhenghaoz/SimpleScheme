#include <iostream>
#include "variable.h"
#include "exception.h"

using namespace std;
using namespace eval;

int main(int argc, char const *argv[])
{
	try {
		Variable a = cons(Variable(1), Variable(2));
		Variable b = cons(Variable(3), Variable(4));
		Variable c = cons(Variable(5), Variable());
		cout << a << endl;
		cout << b << endl;
		cout << c << endl;
		setCdr(a, b);
		setCdr(b, c);
		cout << a << endl;
	} catch (SchemeException e) {
		cout << e.what() << endl;
	}
	return 0;
}