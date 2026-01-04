# Dana Compiler Architecture

## Overview

The Dana compiler follows multi-pass architecture:

```
Source Code (.dana)
        │
        ▼
┌───────────────┐
│    Lexer      │  Tokenization with layout handling
└───────┬───────┘
        │ Token Stream
        ▼
┌───────────────┐
│    Parser     │  Bison-based LALR(1) parser
└───────┬───────┘
        │ Abstract Syntax Tree
        ▼
┌───────────────┐
│  Semantic     │  Type checking, symbol resolution
│  Analysis     │  Control flow validation
└───────┬───────┘
        │ Annotated AST
        ▼
┌───────────────┐
│   Code        │  LLVM IR generation
│   Generation  │
└───────┬───────┘
        │ LLVM IR
        ▼
┌───────────────┐
│  Optimizer    │  LLVM optimization passes
└───────┬───────┘
        │
        ▼
┌───────────────┐
│  LLVM Backend │  Assembly/Object generation
└───────┬───────┘
        │
        ▼
   Executable (a.out)
```

## Directory Structure

```
src/
├── main.cpp                 # Compiler driver
├── Makefile                 # Build configuration
│
├── frontend/
│   ├── lexer/
│   │   └── lexer.l         # Flex lexer with layout rules
│   │
│   ├── parser/
│   │   ├── parser.y        # Bison grammar
│   │   └── parser.tab.*    # Generated parser
│   │
│   ├── ast/
│   │   ├── ast.hpp         # AST node definitions
│   │   ├── ast.cpp         # AST implementation
│   │   ├── ast_visitor.hpp # Visitor interface
│   │   ├── ast_print.cpp   # Pretty printer
│   │   └── operators.hpp   # Operator enums
│   │
│   ├── symbol/
│   │   ├── symbol.hpp      # Symbol hierarchy
│   │   ├── symbol_table.hpp # Symbol table
│   │   ├── scope.hpp       # Scope management
│   │   └── sematype.hpp    # Type representations
│   │
│   ├── semantic/
│   │   ├── semantic.hpp    # Semantic pass entry point
│   │   ├── semantic_pass.* # Type checking pass
│   │   ├── control_flow.*  # Control flow validation
│   │   ├── sema_context.*  # Analysis context
│   │   └── builtins.*      # Built-in function setup
│   │
│   └── common/
│       └── diagnostics.*   # Error reporting
│
├── backend/
│   ├── codegen/
│   │   ├── codegen.hpp     # Code generation interface
│   │   ├── codegen_*.cpp   # Code gen implementations
│   │   └── codegen_context.* # LLVM context management
│   │
│   └── optimizer/
│       └── optimizer.*     # LLVM optimization wrapper
│
└── runtime/
    ├── lib.c               # Runtime library source
    ├── lib_bitcode.hpp     # Embedded runtime bitcode
    └── danalib.*           # Built-in function codegen
```

## Frontend

### Lexer (`frontend/lexer/lexer.l`)

The lexer is implemented using Flex and handles:

1. **Token Recognition**: Keywords, identifiers, literals, operators
2. **Layout Management**: Indentation-based block inference using a guide stack
3. **Comment Handling**: Both `--` line comments and `(* *)` nested block comments
4. **Escape Sequences**: In strings and character literals

Key features:
- **Guide Stack**: Tracks indentation levels for layout-sensitive parsing
- **Auto-End Tokens**: Automatically inserted `T_AUTO_END` tokens when dedenting
- **Location Tracking**: Line and column numbers for error reporting

### Parser (`frontend/parser/parser.y`)

The parser is implemented using Bison with the C++ skeleton and handles:

1. **Grammar**: LALR(1) grammar for Dana
2. **AST Construction**: Builds typed AST nodes
3. **Error Handling**: Reports syntax errors with location

### Abstract Syntax Tree (`frontend/ast/`)

The AST uses a class hierarchy with visitor pattern:

```
AstNode (base)
├── Type
├── FParType
├── Program
├── Def
│   ├── VarDef
│   ├── FuncDecl
│   └── FuncDef
├── Header
├── FParDef
├── Stmt
│   ├── SkipStmt
│   ├── ExitStmt
│   ├── BreakStmt
│   ├── ContinueStmt
│   ├── AssignStmt
│   ├── ReturnStmt
│   ├── ProcCall
│   ├── IfStmt
│   └── LoopStmt
├── Block
├── Lval
│   ├── IdLVal
│   ├── StringLiteralLVal
│   └── IndexLVal
├── Expr
│   ├── IntConst
│   ├── CharConst
│   ├── TrueConst
│   ├── FalseConst
│   ├── LValueExpr
│   ├── ParenExpr
│   ├── FuncCall
│   ├── UnaryExpr
│   └── BinaryExpr
└── Cond
    ├── ExprCond
    ├── ParenCond
    ├── NotCond
    ├── BinaryCond
    └── RelCond
```

### Symbol System (`frontend/symbol/`)

#### Symbol Hierarchy

```
Symbol (base)
├── VarSymbol      # Local variables
├── ParamSymbol    # Function parameters
└── FuncSymbol     # Functions and procedures
```

Each symbol stores:
- Name and location
- Type information
- Defining function (for closure analysis)

#### Type System (`sematype.hpp`)

```
SemaType (base)
├── IntType        # int
├── ByteType       # byte
├── VoidType       # procedure return
├── ArrayType      # T[N] or T[]
└── FuncType       # (params) -> return
```

Types are interned (shared) for efficient comparison.

#### Symbol Table (`symbol_table.hpp`)

- Manages nested scopes
- Handles symbol lookup with shadowing
- Supports forward declarations

### Semantic Analysis (`frontend/semantic/`)

Two-pass semantic analysis:

#### SemanticPass

Performs type checking and symbol resolution:

1. **Type Resolution**: Converts AST types to semantic types
2. **Symbol Declaration**: Registers variables, functions
3. **Type Checking**: Validates operations, assignments, calls
4. **Forward Declaration Handling**: Matches declarations with definitions

#### ControlFlowPass

Validates control flow:

1. **Return/Exit Validation**: Ensures proper placement
2. **Break/Continue Validation**: Checks loop context
3. **Reachability Analysis**: Ensures functions return values
4. **Loop Label Validation**: Checks labeled break/continue

## Backend

### Code Generation (`backend/codegen/`)

LLVM-based code generation with organized modules:

- **codegen_decl.cpp**: Variable and function declarations
- **codegen_stmt.cpp**: Statement code generation
- **codegen_expr.cpp**: Expression code generation
- **codegen_cond.cpp**: Condition code generation
- **codegen_call.cpp**: Function/procedure calls
- **codegen_loop.cpp**: Loop constructs

The `CodegenContext` manages:
- LLVM context, module, and IR builder
- Symbol-to-value mapping
- Current function context
- Loop break/continue targets

### Optimization (`backend/optimizer/`)

Uses LLVM's optimization pipeline:

- `-O0`: No optimization
- `-O1`: Basic optimizations
- `-O2`: Standard optimizations
- `-O3`: Aggressive optimizations

### Runtime Library (`runtime/`)

Written in C, compiled to LLVM bitcode and embedded:

- I/O functions: `writeInteger`, `readString`, etc.
- String functions: `strlen`, `strcmp`, etc.
- Type conversion: `extend`, `shrink`

## Compilation Pipeline

1. **Lexing**: Source → Tokens (with layout handling)
2. **Parsing**: Tokens → AST
3. **Semantic Pass**: Type checking, symbol resolution
4. **Control Flow Pass**: Validate control structures
5. **Code Generation**: AST → LLVM IR
6. **Optimization**: Apply LLVM passes
7. **Linking**: Link with runtime library
8. **Backend**: LLVM IR → Assembly → Object → Executable

## Error Handling

The `Diagnostics` class provides robust error handling, providing the error location in source file, the compilation stage where the error occured and the severity of it (possible classifications: Note, Warning, Error)
