CC := g++

SRCDIR := src
BUILDDIR := build
TARGET := bin
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -std=c++11 -g -Wall -Wextra -pedantic
LIB := -lGL -lGLEW -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
INC := 

$(TARGET): $(OBJECTS)
	export LD_LIBRARY_PATH=~/Documents/glew/test/glew-2.0.0/lib
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB) 2> link_error.log

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	export LD_LIBRARY_PATH=~/Documents/glew/test/glew-2.0.0/lib
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $< 2> build_error.log

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

.PHONY: clean
