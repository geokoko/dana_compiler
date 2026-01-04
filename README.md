# Dana Compiler

A compiler for the Dana programming language.
Dana is an educational programming language designed as a semester project for the Compilers Course (8th semester) in ECE NTUA.

[![Tests](https://img.shields.io/badge/tests-35%2F36%20passing-green)]()
[![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)]()
[![LLVM](https://img.shields.io/badge/LLVM-18-orange)]()

## Features

- **Frontend**: Lexer (Flex) with layout handling, LALR(1) parser (Bison), AST construction
- **Semantic Analysis**: Type checking, symbol resolution, control flow validation
- **LLVM Backend**: Code generation, optimization passes
- **Dana Runtime Library**: Built-in functions, with similar behavior to their C counterparts.

## Documentation

Comprehensive documentation is available in the [docs/](docs/) folder:

- [Language Reference](docs/language-reference.md) - Complete Dana language specification
- [Architecture Overview](docs/architecture.md) - Compiler architecture and design
- [Building and Usage](docs/building.md) - How to build and use the compiler
- [API Reference](docs/api-reference.md) - Internal API documentation

## Quick Start

```bash
# Build the compiler
cd src && make

# Compile a Dana program
./danac <program_name>.dana

# Run the generated executable
./a.out
``` 

## Example Programs

```dana
def hello
   writeString: "Hello world!\n"
```

```dana
def main
    def factorial is int: n as int
        if n <= 1: return: 1
        else: return: n * factorial(n - 1)
    
    var n result is int
    n := readInteger()
    result := factorial(n)
    writeString: "Factorial: "
    writeInteger: result
    writeString: "\n"
```

For more examples, see the [tests/programs](../tests/programs/) directory.

## Project Structure
```
dana_compiler/
├── src/
│   ├── frontend/             # Lexer, parser, AST, semantic analysis
│   ├── backend/              # Code generation, optimization
│   ├── runtime/              # Runtime library
│   └── main.cpp              # Compiler driver
├── tests/
│   ├── programs/             # Valid test programs
│   └── programs-erroneous/   # Programs that should fail
├── docs/                     # Documentation
└── dana2025.pdf              # Language specification
```

## Building

### Prerequisites

- Clang (with C++17 support)
- LLVM 18 (`llvm-config-18`)
- Flex & Bison
- Python 3 with pytest (for tests)

### Build Commands

```bash
cd src
make            # Build compiler from source
make clean      # Clean all object files, generated IR, generated assembly
```

## Running Tests

```bash
pytest tests/ -v
```