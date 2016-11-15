# 
# Makefile for the SimpleScheme project
# 
# Author: Zhang Zhanghao (zhangzhenghao@hotmail.com)
#

#
# Global variables
#
CC			= clang++
BUILD_DIR	= ../build/
YACC		= bison -dy
LEX			= flex

CPPFLAGS	= -g -O1 -Wall -std=c++11

SOURCES		= variable.cpp test1.cpp
OBJECTS		= $(addprefix $(BUILD_DIR), $(SOURCES:.cpp=.o))
DEPENDENCES	= $(addprefix $(BUILD_DIR), $(SOURCES:.cpp=.d))

# 
# Build target
# 
test1: $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) -o test1; \
	./test1

all:
	echo $(SOURCES)

clean:
	rm $(OBJECTS) $(DEPENDENCES)

#
# Rules
#
-include $(DEPENDENCES)

$(BUILD_DIR)%.d: %.cpp
	@set -e; \
	 rm -f $@; \
	 $(CC) $(CPPFLAGS) -MM -MT $(@:.d=.o) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' $@.$$$$ > $@; \
	 rm -f $@.$$$$

$(BUILD_DIR)%.o: %.cpp
	$(CC) $(CPPFLAGS) $< -c -o $@
