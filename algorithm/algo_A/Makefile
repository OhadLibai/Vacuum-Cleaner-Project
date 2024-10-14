# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -g -rdynamic -fPIC -O3 -I$(SRCDIR)/algorithm/algo_A

# Directories
SRCDIR = ../..
OBJDIR = ../../obj
BINDIR = ../../bin

# Shared Libraries
ALGO_LIB1 = $(BINDIR)/208825547_206843328_A_algo.so

# Algorithm Source files
ALGO_SOURCES = $(SRCDIR)/algorithm/algo_A/algo_A.cpp $(SRCDIR)/algorithm/MyAlgorithm.cpp 

# Algorithm Object files
ALGO_OBJECTS = $(patsubst $(SRCDIR)/algorithm/algo_A%.cpp, $(OBJDIR)/algorithm/algo_A%.o, $(ALGO_SOURCES))

# Default target
all: $(ALGO_LIB1)

# Build the shared library
$(ALGO_LIB1): $(ALGO_OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

# Compile algo_A source files into object files
$(OBJDIR)/algorithm/algo_A/%.o: $(SRCDIR)/algorithm/algo_A/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile MyAlgorithm source file into an object file
$(OBJDIR)/algorithm/%.o: $(SRCDIR)/algorithm/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJDIR)/algorithm/algo_A $(BINDIR)/208825547_206843328_A_algo.so

# Phony targets
.PHONY: all clean
