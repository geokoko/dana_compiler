# Dana Compiler

A compiler for the Dana programming language.
Dana is an educational programming language designed as a semester project for the Compilers Course (8th semester) in ECE NTUA.

[![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)](https://en.cppreference.com/w/cpp/17)
[![LLVM](https://img.shields.io/badge/LLVM-18-orange)](https://llvm.org/)

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

## Quick Start

### Option 1: Pre-built Binary (Recommended - No Build Required)

Download the latest release from [GitHub Releases](https://github.com/geokoko/dana_compiler/releases):

```bash
# Download and extract
wget https://github.com/geokoko/dana_compiler/releases/latest/download/danac-linux-x64.tar.gz
tar xzf danac-linux-x64.tar.gz

# Install system-wide
sudo mv danac /usr/local/bin/

# Use it!
danac myprogram.dana && ./a.out
```


### Option 2: Build from Source

```bash
git clone https://github.com/geokoko/dana_compiler.git
cd dana_compiler

# Install dependencies (auto-detects your distro)
make deps

# Build and install
sudo make install

# Use it!
danac myprogram.dana && ./a.out
```

#### Dependencies installed automatically

- Clang (with C++17 support)
- LLVM 18 (`llvm-config-18`)
- Flex & Bison
- Python 3 with pytest (for tests)

#### Build Commands

```bash
make            # Build compiler
make install    # Install to /usr/local/bin (needs sudo)
make uninstall  # Remove installation
make test       # Run test suite
make clean      # Clean build artifacts
```
