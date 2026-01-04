# Building and Using the Dana Compiler

## Prerequisites

### Required Software

- **C++ Compiler**: Clang with C++17 support
- **LLVM**: Version 18 (headers and libraries)
- **Flex**: Lexer generator
- **Bison**: Parser generator (version 3.8+)
- **Python 3**: For running tests (pytest)

### Installation on Fedora/RHEL

```bash
sudo dnf install llvm18-devel clang flex bison gcc-c++ python3-pytest
```

### Installation on Ubuntu/Debian

```bash
sudo apt install llvm-18-dev clang flex bison g++ python3-pytest
```

## Building the Compiler

```bash
cd src
make
```

This produces the `danac` compiler executable.

### Build Targets

```bash
make              # Build the compiler
make clean        # Remove all generated files
```

## Using the Compiler

### Basic Usage

```bash
./danac <source.dana>
```

This compiles `source.dana` and produces:
- `source.imm` - LLVM IR (intermediate representation)
- `source.asm` - Assembly code
- `source.o` - Object file
- `a.out` - Executable (for now, every executable has this name)

### Command-Line Options

| Option       | Description                        |
|--------------|------------------------------------|
| `--ast-tree` | Print the AST before compilation   |
| `--emit-ir`  | Print LLVM IR to stdout            |
| `-O0`        | No optimization (default)          |
| `-O1`        | Basic optimization                 |
| `-O2`        | Standard optimization              |
| `-O3`        | Aggressive optimization            |

### Examples

```bash
# Compile with optimization
./danac -O2 program.dana

# View the AST
./danac --ast-tree program.dana

# View generated LLVM IR
./danac --emit-ir program.dana

# Compile and run
./danac program.dana && ./a.out
```

## Running Programs

### Standard Input/Output

Dana programs use standard I/O:

```bash
# Run with input from terminal
./a.out

# Run with input from file
./a.out < input.txt

# Run with input and capture output
./a.out < input.txt > output.txt
```

### Example Session

```bash
$ cat hello.dana
def main
   writeString: "Hello, world!\n"

$ ./danac hello.dana
Done.
Generated:
  Intermediate: hello.imm
  Assembly:     hello.asm
  Object:       hello.o
  Executable:   ./a.out

$ ./a.out
Hello, world!
```

## Running Tests

### Test Suite

The compiler includes a test suite using pytest:

```bash
cd /path/to/dana_compiler
pytest tests/ -v
```

### Test Categories

1. **Valid Programs** (`tests/programs/`):
   - Programs that should compile and run successfully
   - Includes `.input` files for stdin
   - Includes `.result` files for expected output

2. **Erroneous Programs** (`tests/programs-erroneous/`):
   - Programs that should fail during compilation
   - Tests error detection capabilities

### Running Individual Tests

```bash
# Run specific test
pytest tests/test_compiler.py::test_dana_program[hello.dana] -v

# Run with debugging output
pytest tests/ -v -s
```

### Adding New Tests

1. Add a `.dana` file to `tests/programs/`
2. Optionally add `.input` file for stdin
3. Add `.result` file with expected output

Example:
```
tests/programs/
  mytest.dana      # Dana source code
  mytest.input     # Input data (optional)
  mytest.result    # Expected output
```

## Troubleshooting

### Common Build Errors

**LLVM not found:**
```
fatal error: llvm/IR/Module.h: No such file or directory
```
Solution: Install LLVM development packages or set `LLVM_CONFIG`:
```bash
export LLVM_CONFIG=/usr/bin/llvm-config-18
```

**Bison version too old:**
```
%require "3.8" is not supported
```
Solution: Install Bison 3.8+:
```bash
# Build from source if needed
wget http://ftp.gnu.org/gnu/bison/bison-3.8.tar.gz
tar xf bison-3.8.tar.gz && cd bison-3.8
./configure && make && sudo make install
```
