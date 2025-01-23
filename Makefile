# Define the compiler
CXX = g++
CXXFLAGS = -std=c++11

# Define the directories
SRCDIR = ./main

# Define the source files
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/timer_functions.cpp

# Define the output binary
BINARY = scrambler

# Default target
all: $(BINARY)

# Link the binary
$(BINARY): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean up the build
clean:
	rm -f $(BINARY)

