# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

BISON := bison
BISONFLAGS := -Wall -d --skeleton=lalr1.cc
FLEX := flex
LEXFLAGS := --header-file=frontend/lexer/lexer.hpp

# Target executable
TARGET = danac

# Source directories
FRONTEND_DIR = frontend
LEXER_DIR = $(FRONTEND_DIR)/lexer
PARSER_DIR = $(FRONTEND_DIR)/parser
AST_DIR = $(FRONTEND_DIR)/ast
TEST_DIR = ../tests

# Generated files
LEXER_CPP = $(LEXER_DIR)/lexer.cpp
LEXER_H = $(LEXER_DIR)/lexer.hpp
PARSER_CPP = $(PARSER_DIR)/parser.tab.cc
PARSER_HH = $(PARSER_DIR)/parser.tab.hh
PARSER_LOC = $(PARSER_DIR)/location.hh

SRCS := main.cpp $(AST_DIR)/ast.cpp $(AST_DIR)/ast_print.cpp $(PARSER_CPP) $(LEXER_CPP)
OBJS := $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Build main compiler executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files
%.o: %.cpp $(PARSER_HH) $(LEXER_H)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Generate parser with Bison
$(PARSER_CPP) $(PARSER_HH): $(PARSER_DIR)/parser.y
	$(BISON) $(BISONFLAGS) -o $(PARSER_CPP) $<

# Generate lexer with Flex
$(LEXER_CPP) $(LEXER_H): $(LEXER_DIR)/lexer.l
	$(FLEX) $(LEXFLAGS) -o $(LEXER_CPP) $<

# Clean intermediate files
clean:
	rm -f $(OBJS) $(TARGET) \
	      $(PARSER_CPP) $(PARSER_HH) \
	      $(LEXER_CPP) $(LEXER_H) \
		  $(PARSER_LOC)

# Clean everything including the executable
distclean: clean
	rm -f $(TARGET)

# Test targets - using test programs from tests/ directory
test: $(TARGET)
	@echo "Testing with hello.dana..."
	./$(TARGET) < $(TEST_DIR)/hello.dana

test-hanoi: $(TARGET)
	@echo "Testing with hanoi.dana..."
	./$(TARGET) < $(TEST_DIR)/hanoi.dana

.PHONY: all clean distclean test test-hanoi 
