CC			= g++
CPPFLAGS	= -std=c++11
SRCDIR		= src
BUILDDIR	= build
EXECUTABLE 	= $(BUILDDIR)/main
SOURCES		= $(wildcard $(SRCDIR)/*.cpp)
OBJECTS		= $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))
DEPENDECES	= $(OBJECTS:.o=.d)

all: $(EXECUTABLE)

# debug

debug: CPPFLAGS += -D EVAL_TRACE_MEMORY
debug: $(EXECUTABLE)

# compile

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) -o $(EXECUTABLE)

-include $(DEPENDECES)

$(BUILDDIR)/%.d: $(SRCDIR)/%.cpp
	$(CC) -MM $(CPPFLAGS) $< > $@
	sed -i 's,\($*\)\.o[ :]*,\1.o $@ : ,g' $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CPPFLAGS) $< -c -o $@

# clean

.PHONY: clean
clean:
	-rm $(OBJECTS) $(DEPENDECES)