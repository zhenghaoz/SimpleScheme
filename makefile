CC			= g++
CPPFLAGS	= -std=c++11 -pg
OUTPUT 		= main
SOURCES		= $(wildcard *.cpp)
OBJECTS		= $(SOURCES:.cpp=.o)
DEPENDECES	= $(OBJECTS:.o=.d)

# compile

$(OUTPUT): $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) -o $(OUTPUT)

-include $(DEPENDECES)

%.d: %.cpp
	$(CC) $(CPPFLAGS) -MM $< > $@

# run

.PHONY: run
run: $(OUTPUT)
	./$(OUTPUT)

# clean

.PHONY: clean
clean:
	-rm $(OBJECTS) $(DEPENDECES) $(OUTPUT)