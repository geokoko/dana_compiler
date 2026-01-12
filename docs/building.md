# Building and Installing the Dana Compiler

## Quick Installation (Automated)

The easiest way - the Makefile detects your package manager and installs dependencies automatically:

```bash
# Clone the repository
git clone https://github.com/geokoko/dana_compiler.git
cd dana_compiler

# Install dependencies (auto-detects package manager)
# Supports: apt, dnf, yum
make deps

# Build and install system-wide
sudo make install

# Verify installation
danac --help
```

The `make deps` target automatically detects your package manager and installs the required packages. Supported distributions:
- **Ubuntu/Debian** (apt)
- **Fedora/RHEL** (dnf/yum)

---

## Manual Installation

### Required Software

- **Build essentials**: gcc, g++, make
- **C++ Compiler**: Clang 18 with C++17 support
- **LLVM**: Version 18 (headers and libraries)
- **Flex**: Lexer generator
- **Bison**: Parser generator (version 3.8+)
- **Python 3**: With pip and pytest for running tests
- **wget**: For downloading LLVM installation script

### Installation on Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y build-essential bison flex python3 python3-pip wget

# Install LLVM 18 using official script
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18
sudo apt-get update
sudo apt-get install -y llvm-18 llvm-18-dev libllvm18 clang-18 libclang-rt-18-dev

# Create symlinks
sudo ln -sf /usr/bin/clang-18 /usr/bin/clang
sudo ln -sf /usr/bin/clang++-18 /usr/bin/clang++
rm llvm.sh

# Install pytest
pip3 install pytest
```

### Installation on Fedora/RHEL

```bash
sudo dnf install -y clang llvm18 llvm18-devel llvm18-static flex bison gcc-c++ python3 python3-pip
pip3 install --user pytest
```

> [!NOTE]
> Fedora may not have LLVM 18 in all versions. Ubuntu/Debian is recommended for guaranteed LLVM 18 support.

## Detailed Installation Steps

The easiest way to get started:

```bash
# Clone the repository
git clone https://github.com/geokoko/dana_compiler.git
cd dana_compiler

# Install system-wide (builds and installs in one command)
sudo make install

# Or build and install to your home directory (no sudo needed)
make PREFIX=~/.local install

# Verify installation
danac --help

# Fallback: If the above don't work, run:
cd src
make
# and compile with
./danac <program>.dana
```

**Note:** The `install` target automatically builds the compiler if needed, so you don't need to run `make` separately.

After installation, `danac` is available from anywhere in your system.

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
danac -O2 program.dana

# View the AST
danac --ast-tree program.dana

# View generated LLVM IR
danac --emit-ir program.dana

# Compile and run
danac program.dana && ./a.out
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

$ danac hello.dana
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

# Build the compiler first
make

# Run tests
make test
# or directly:
pytest tests/ -v
```

**Pytest auto-build behavior:** When you run `pytest tests/ -v` directly, the test fixture will rebuild the compiler (`make clean && make`) unless you provide a prebuilt binary via `CI_PREBUILT_EXECUTABLE=/path/to/danac` (e.g., `CI_PREBUILT_EXECUTABLE=src/danac`). Use that env var to skip rebuilding once you already have a compiled `danac`. 

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

## Makefile Targets

| Target | Description |
|--------|-------------|
| `make` | Build the compiler |
| `make deps` | Install dependencies (auto-detects package manager) |
| `make deps-ubuntu` | Install Ubuntu/Debian dependencies (apt) |
| `make deps-fedora` | Install Fedora/RHEL dependencies (dnf) |
| `make install` | Build and install to `/usr/local/bin` (requires sudo) |
| `make uninstall` | Remove from `/usr/local/bin` |
| `make test` | Run test suite (requires prior build) |
| `make clean` | Remove build artifacts |

## Uninstalling

To remove the installed compiler:

```bash
# From system installation
sudo make uninstall

# From local installation
make PREFIX=~/.local uninstall
```

Or manually remove:
```bash
rm /usr/local/bin/danac        # System install
rm ~/.local/bin/danac          # User install
```
