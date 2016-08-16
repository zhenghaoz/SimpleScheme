CC			= g++
CPPFLAGS	= -std=c++11
OUTPUT 		= main
SOURCES		= $(wildcard *.cpp)
OBJECTS		= $(SOURCES:.cpp=.o)
DEPENDECES	= $(OBJECTS:.o=.d)

# compile

$(OUTPUT): $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) -o $(OUTPUT)

-include $(DEPENDECES)

%.d: %.cpp
	$(CC) -MM $(CPPFLAGS) $< > $@
	sed -i 's,\($*\)\.o[ :]*,\1.o $@ : ,g' $@

# clean

.PHONY: clean
clean:
	-rm $(OBJECTS) $(DEPENDECES) $(OUTPUT)