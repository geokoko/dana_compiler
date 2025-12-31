# Toolchain
CXX = clang++
CC = clang
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -g -Wno-unused-parameter
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -g

# LLVM flags (using llvm 18)
LLVM_CONFIG = llvm-config-18
# Compile C++ using LLVM
# Use -isystem instead of -I to suppress warnings from LLVM headers
LLVM_CXXFLAGS = $(shell $(LLVM_CONFIG) --cxxflags | sed 's/-I/-isystem /g')
# Link LLVM libs and POSIX libs
LLVM_LDFLAGS  = $(shell $(LLVM_CONFIG) --ldflags --libs core native) \
				$(shell $(LLVM_CONFIG) --system-libs)

# Add rpath so danac finds libLLVM at runtime
# This flag enables the user to run danac executable without LD_LIBRARY_PATH
LLVM_RPATH = -Wl,-rpath,$(shell $(LLVM_CONFIG) --libdir)

# Append LLVM flags to global gcc flags
CXXFLAGS += $(LLVM_CXXFLAGS)

# Bison and Flex settings
BISON := bison
BISONFLAGS := -Wall -d --skeleton=lalr1.cc
FLEX := flex
LEXFLAGS := --header-file=frontend/lexer/lexer.hpp

# Target executable
TARGET = danac

# Source directories
FRONTEND_DIR = frontend
BACKEND_DIR = backend
RUNTIME_DIR = runtime
LEXER_DIR = $(FRONTEND_DIR)/lexer
PARSER_DIR = $(FRONTEND_DIR)/parser
AST_DIR = $(FRONTEND_DIR)/ast
SEMANTIC_DIR = $(FRONTEND_DIR)/semantic
SYMBOL_DIR = $(FRONTEND_DIR)/symbol
TEST_DIR = ../tests/programs

# Generated files
LEXER_CPP = $(LEXER_DIR)/lexer.cpp
LEXER_H = $(LEXER_DIR)/lexer.hpp
PARSER_CPP = $(PARSER_DIR)/parser.tab.cc
PARSER_HH = $(PARSER_DIR)/parser.tab.hh
PARSER_LOC = $(PARSER_DIR)/location.hh

# Default target
all: $(TARGET)

# cpp source files
SRCS := main.cpp \
		$(AST_DIR)/ast.cpp \
		$(AST_DIR)/ast_print.cpp \
		$(SEMANTIC_DIR)/semantic.cpp \
		$(SEMANTIC_DIR)/sema_context.cpp \
		$(SEMANTIC_DIR)/diagnostics.cpp \
		$(SYMBOL_DIR)/symbol.cpp \
		$(SYMBOL_DIR)/symbol_table.cpp \
		$(SYMBOL_DIR)/scope.cpp \
		$(SYMBOL_DIR)/sematype.cpp \
		$(BACKEND_DIR)/codegen/codegen_context.cpp \
		$(BACKEND_DIR)/codegen/codegen_call.cpp \
		$(BACKEND_DIR)/codegen/codegen_cond.cpp \
		$(BACKEND_DIR)/codegen/codegen_decl.cpp \
		$(BACKEND_DIR)/codegen/codegen_expr.cpp \
		$(BACKEND_DIR)/codegen/codegen_loop.cpp \
		$(BACKEND_DIR)/codegen/codegen_stmt.cpp \
		$(BACKEND_DIR)/optimizer/optimizer.cpp \
		${RUNTIME_DIR}/danalib.cpp \
		$(PARSER_CPP) \
		$(LEXER_CPP)

# build library of C functions
runtime/lib.o: runtime/lib.c
	$(CC) $(CFLAGS) -c -o $@ $<

# object files
OBJS := $(SRCS:.cpp=.o)

# Link to LLVM and build main compiler executable
$(TARGET): $(OBJS) runtime/lib.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LLVM_LDFLAGS) $(LLVM_RPATH)

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
	rm -f $(TEST_DIR)/*.imm \
		$(TEST_DIR)/*.asm \
		$(TEST_DIR)/*.o \
		*.out \
		runtime/lib.o \
		$(OBJS) $(TARGET) \
		$(PARSER_CPP) $(PARSER_HH) \
		$(LEXER_CPP) $(LEXER_H) \
		$(PARSER_LOC)

# Clean everything including the executable
distclean: clean

.PHONY: all clean distclean 
