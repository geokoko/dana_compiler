# Toolchain
CXX = clang++
CC = clang
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -g -Wno-unused-parameter
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -g

# LLVM flags (using llvm 18)
LLVM_CONFIG = llvm-config-18
# Use llvm-as from the same LLVM install to keep bitcode version compatible.
LLVM_BINDIR = $(shell $(LLVM_CONFIG) --bindir)
LLVM_AS = $(LLVM_BINDIR)/llvm-as
# Compile C++ using LLVM
# Use -isystem instead of -I to suppress warnings from LLVM headers
LLVM_CXXFLAGS = $(shell $(LLVM_CONFIG) --cxxflags --link-static | sed 's/-I/-isystem /g')
# Link LLVM libs and POSIX libs
LLVM_LDFLAGS  = $(shell $(LLVM_CONFIG) --ldflags --link-static --libs core native passes) \
				$(shell $(LLVM_CONFIG) --link-static --system-libs)

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
RUNTIME_BC = $(RUNTIME_DIR)/lib.bc
RUNTIME_LL = $(RUNTIME_DIR)/lib.ll
RUNTIME_BC_HDR = $(RUNTIME_DIR)/lib_bitcode.hpp
RUNTIME_EMBED = $(RUNTIME_DIR)/embed_bitcode.py

# Default target
all: $(TARGET)

# cpp source files
SRCS := main.cpp \
		$(AST_DIR)/ast.cpp \
		$(AST_DIR)/ast_print.cpp \
		$(FRONTEND_DIR)/common/diagnostics.cpp \
		$(SEMANTIC_DIR)/semantic_pass.cpp \
		$(SEMANTIC_DIR)/control_flow.cpp \
		$(SEMANTIC_DIR)/sema_context.cpp \
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

$(RUNTIME_LL): $(RUNTIME_DIR)/lib.c
	$(CC) $(CFLAGS) -g0 -S -emit-llvm -o $@ $<

$(RUNTIME_BC): $(RUNTIME_LL)
	$(LLVM_AS) -o $@ $<

$(RUNTIME_BC_HDR): $(RUNTIME_BC) $(RUNTIME_EMBED)
	python3 $(RUNTIME_EMBED) $< $@

# object files
OBJS := $(SRCS:.cpp=.o)

# Link to LLVM and build main compiler executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LLVM_LDFLAGS) $(LLVM_RPATH)

# Compile source files
%.o: %.cpp $(PARSER_HH) $(LEXER_H)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main.o: $(RUNTIME_BC_HDR)

# Generate parser with Bison
$(PARSER_CPP) $(PARSER_HH): $(PARSER_DIR)/parser.y
	$(BISON) $(BISONFLAGS) -o $(PARSER_CPP) $<

# Generate lexer with Flex
$(LEXER_CPP) $(LEXER_H): $(LEXER_DIR)/lexer.l
	$(FLEX) $(LEXFLAGS) -o $(LEXER_CPP) $<

# Clean intermediate files
clean:
	rm -f $(TEST_DIR)/*.imm \
	rm -f $(TEST_DIR)/*.asm \
	rm -f $(TEST_DIR)/*.o \
	rm -f *.out \
	rm -f runtime/lib.o \
	rm -f $(RUNTIME_BC) \
	rm -f $(RUNTIME_LL) \
	rm -f $(RUNTIME_BC_HDR) \
		$(OBJS) $(TARGET) \
		$(PARSER_CPP) $(PARSER_HH) \
		$(LEXER_CPP) $(LEXER_H) \
		$(PARSER_LOC)

.PHONY: all clean 
