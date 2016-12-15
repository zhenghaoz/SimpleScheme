# 
# Makefile for the SimpleScheme project
# 
# Author: Zhang Zhanghao (zhangzhenghao@hotmail.com)
#

#
# Global option
#
CC				= clang++
BUILD_DIR		= ../build/
LIB_DIR			= ../lib/
BIN_DIR			= ../bin/
AR 				= ar -rc
RANLIB      	= ranlib
YACC			= bison -dy
LEX				= flex

# 
# Build option
# 
CPPFLAGS		= -g -O1 -Wall -std=c++11 $(STATS_OPTION)
STATS_OPTION	= -DSTATS
LIB 			= -lsscheme 

# 
# Files
# 
SOURCES			= variable.cpp environment.cpp evaluator.cpp primitive.cpp garbage.cpp statistic.cpp $(PARSER_SRC)
OBJECTS			= $(addprefix $(BUILD_DIR), $(SOURCES:.cpp=.o))
DEPENDENCES		= $(addprefix $(BUILD_DIR), $(SOURCES:.cpp=.d))
EXECUTE			= $(BIN_DIR)main
LIBRARY			= $(LIB_DIR)libsscheme.a
PARSER_SRC		= lexer.cpp parser.cpp
PARSER_HD		= parser.hpp

# 
# Build target
# 

all: $(LIBRARY) $(EXECUTE)

get-deps:
	sudo apt-get install libboost-all-dev

$(EXECUTE): main.cpp $(LIBRARY) 
	$(CC) $(CPPFLAGS) $(OBJECTS) main.cpp -o $(EXECUTE) -L$(LIB_DIR) $(LIB)

$(LIBRARY): $(OBJECTS)
	$(AR) $(LIBRARY) $(OBJECTS); \
	$(RANLIB) $(LIBRARY)

parser.cpp: parser.yacc
	$(YACC) -o parser.cpp parser.yacc

lexer.cpp: lexer.lex
	$(LEX) -o lexer.cpp lexer.lex 

clean:
	rm $(OBJECTS) $(DEPENDENCES) $(EXECUTE) $(LIBRARY) $(PARSER_SRC) $(PARSER_HD)

-include $(DEPENDENCES)

#
# Rules
#

$(BUILD_DIR)%.d: %.cpp
	@set -e; \
	 rm -f $@; \
	 $(CC) $(CPPFLAGS) -MM -MT $(@:.d=.o) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' $@.$$$$ > $@; \
	 rm -f $@.$$$$

$(BUILD_DIR)%.o: %.cpp
	$(CC) $(CPPFLAGS) $< -c -o $@
