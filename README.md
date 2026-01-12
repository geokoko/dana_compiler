# Dana Compiler

A compiler for the Dana programming language.
Dana is an educational programming language designed as a semester project for the Compilers Course (8th semester) in ECE NTUA.

[![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)](https://en.cppreference.com/w/cpp/17)
[![LLVM](https://img.shields.io/badge/LLVM-18-orange)](https://llvm.org/)
[![Bison](https://img.shields.io/badge/Bison-3.8%2B-green)](https://www.gnu.org/software/bison/)
[![Flex](https://img.shields.io/badge/Flex-2.6%2B-yellowgreen)](https://github.com/westes/flex)

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

# Install dependencies (auto-detects your package manager)
# Supports: Ubuntu/Debian (apt), Fedora/RHEL (dnf/yum), Arch (pacman), openSUSE (zypper)
make deps

# Build and install
sudo make install

# Use it!
danac myprogram.dana && ./a.out
```

**Having issues?** See [Manual Installation](docs/building.md#manual-installation-if-make-deps-doesnt-work) for step-by-step instructions.

#### Dependencies installed automatically

- Build essentials (gcc, g++, make)
- Clang 18 (with C++17 support)
- LLVM 18 (development libraries)
- Flex & Bison
- Python 3 with pip and pytest

#### Build Commands

```bash
make            # Build compiler
make deps       # Install dependencies (auto-detects package manager)
make install    # Install to /usr/local/bin (needs sudo)
make uninstall  # Remove installation
make test       # Run test suite (requires prior build)
make clean      # Clean build artifacts
```

#### Supported Distributions

The `make deps` command works on:
- **Ubuntu/Debian** (apt) - LLVM 18 installed via official llvm.sh
- **Fedora/RHEL** (dnf/yum) - uses system LLVM packages

## Usage

### Basic Compilation

```bash
# Compile a Dana program (generates a.out in current directory)
danac program.dana

# Run the compiled program
./a.out
```

### Compiler Flags

```bash
danac [OPTIONS] <file.dana>

Options:
  --ast-tree       Print the abstract syntax tree after parsing
  --emit-ir        Emit LLVM IR to stdout instead of compiling
  -O0              No optimization (default)
  -O1              Basic optimizations
  -O2              Moderate optimizations
  -O3              Aggressive optimizations
```

### Examples

```bash
# Compile with optimizations
danac -O3 program.dana && ./a.out

# View generated LLVM IR
danac --emit-ir program.dana > program.ll

# Debug: view AST structure
danac --ast-tree program.dana

# Compile and run with input
./a.out < input.txt
```
