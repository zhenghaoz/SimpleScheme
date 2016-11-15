#define CATCH_CONFIG_MAIN

#include <string>
#include "variable.hpp"
#include "catch.hpp"

using namespace std;

// Symbol value

TEST_CASE("Symbol value", "[SYMBOL]" ) {
	// Standard output
	Variable var("Hello", Variable::SYMBOL);
	REQUIRE(var.getType() == Variable::SYMBOL);
	REQUIRE(var.toString() == "Hello");
	// Copy construct
	Variable var2(var);
	REQUIRE(var.getType() == Variable::SYMBOL);
	REQUIRE(var2.toString() == "Hello");
	// Assignment
	var2 = Variable("World", Variable::SYMBOL);
	REQUIRE(var.getType() == Variable::SYMBOL);
	REQUIRE(var2.toString() == "World");
}

// String value

TEST_CASE("String value", "[STRING]" ) {
	// Standard output
	Variable var("Hello", Variable::STRING);
	REQUIRE(var.getType() == Variable::STRING);
	REQUIRE(var.toString() == "\"Hello\"");
	// Copy construct
	Variable var2(var);
	REQUIRE(var.getType() == Variable::STRING);
	REQUIRE(var2.toString() == "\"Hello\"");
	// Assignment
	var2 = Variable("World", Variable::STRING);
	REQUIRE(var.getType() == Variable::STRING);
	REQUIRE(var2.toString() == "\"World\"");
}

// Logical operation

TEST_CASE("Logical operation", "[LOGICAL]") {
	true and false;
/*	Variable a = VAR_TRUE;
	Variable b = VAR_FALSE;
	REQUIRE(and(a, a) == VAR_TRUE);*/
}