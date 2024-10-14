# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -g -rdynamic -fPIC -O3 -I$(SRCDIR)

# Directories
SRCDIR = .
OBJDIR = obj
BINDIR = bin

# Executable
TARGET = $(BINDIR)/../myrobot

# Source files
SOURCES = $(wildcard $(SRCDIR)/main.cpp $(SRCDIR)/common/*.cpp $(SRCDIR)/factory/*.cpp $(SRCDIR)/sensors/*.cpp $(SRCDIR)/simulator/*.cpp $(SRCDIR)/testing/*.cpp)

# Object files
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# Default target
all: $(TARGET)

# Separate targets for building algo_A and algo_B
algo_A:
	$(MAKE) -C $(SRCDIR)/algorithm/algo_A

algo_B:
	$(MAKE) -C $(SRCDIR)/algorithm/algo_B

# Build the executable after building algo_A and algo_B
$(TARGET): $(OBJECTS)
	$(MAKE) -C $(SRCDIR)/algorithm/algo_A
	$(MAKE) -C $(SRCDIR)/algorithm/algo_B
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each source file into an object file
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up build artifacts for the main project
clean-main:
	rm -rf $(OBJDIR) $(TARGET)

# Clean up build artifacts for algo_A and algo_B
clean-algo_A:
	$(MAKE) -C $(SRCDIR)/algorithm/algo_A clean

clean-algo_B:
	$(MAKE) -C $(SRCDIR)/algorithm/algo_B clean

# Clean everything
clean: clean-main clean-algo_A clean-algo_B


# Phony targets
.PHONY: all clean clean-main clean-algo_A clean-algo_B algo_A algo_B
