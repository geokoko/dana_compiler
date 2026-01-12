# Dana Compiler - Top-level Makefile
# Usage:
#   make            - Build the compiler
#   make deps       - Install dependencies (detects package manager automatically)
#   make install    - Build and install to /usr/local/bin (requires sudo)
#   make uninstall  - Remove from /usr/local/bin
#   make test       - Run test suite
#   make clean      - Clean build artifacts

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

# Detect Linux distribution / package manager
ifeq ($(shell uname -s),Linux)

  # Package manager detection
  ifneq ($(shell command -v apt-get >/dev/null 2>&1 && echo apt),)
    PKG_MGR := apt
  else ifneq ($(shell command -v dnf >/dev/null 2>&1 && echo dnf),)
    PKG_MGR := dnf
  else ifneq ($(shell command -v yum >/dev/null 2>&1 && echo yum),)
    PKG_MGR := yum
  else
    PKG_MGR := unknown
  endif

else
  PKG_MGR := unknown
endif


.PHONY: all build install uninstall test clean help deps deps-ubuntu deps-fedora

all: build

build:
	$(MAKE) -C src

install: build
	@echo "Installing danac to $(BINDIR)..."
	install -d $(BINDIR)
	install -m 755 src/danac $(BINDIR)/danac
	@echo "Done! You can now run 'danac' from anywhere."

uninstall:
	@echo "Removing danac from $(BINDIR)..."
	rm -f $(BINDIR)/danac
	@echo "Done!"

test:
	pytest tests/ -v

clean:
	$(MAKE) -C src clean

# Detect and install dependencies automatically based on package manager
deps:
	@echo "Detected package manager: $(PKG_MGR)"
	@if [ "$(PKG_MGR)" = "apt" ]; then \
		$(MAKE) deps-ubuntu; \
	elif [ "$(PKG_MGR)" = "dnf" ] || [ "$(PKG_MGR)" = "yum" ]; then \
		$(MAKE) deps-fedora; \
	else \
		echo "Unsupported package manager. Only Ubuntu/Debian (apt) and Fedora/RHEL (dnf/yum) are supported."; \
		echo "Required packages: clang, llvm 18, bison, flex, python3, python3-pip"; \
		exit 1; \
	fi

# Ubuntu/Debian dependencies
deps-ubuntu:
	@echo "Installing dependencies for Ubuntu/Debian..."
	sudo apt-get update
	# lsb-release is required by llvm.sh script, libzstd-dev for linking
	sudo apt-get install -y build-essential bison flex python3 python3-pip wget lsb-release software-properties-common gnupg libzstd-dev
	# Install LLVM 18
	wget https://apt.llvm.org/llvm.sh
	chmod +x llvm.sh
	sudo ./llvm.sh 18
	sudo apt-get update || true
	sudo apt-get install -y llvm-18 llvm-18-dev libllvm18 clang-18 libclang-rt-18-dev
	# Create symlinks
	sudo ln -sf /usr/bin/clang-18 /usr/bin/clang
	sudo ln -sf /usr/bin/clang++-18 /usr/bin/clang++
	rm llvm.sh
	# Install pytest via apt (avoids PEP 668 externally-managed-environment issues)
	sudo apt-get install -y python3-pytest
	@echo "Dependencies installed successfully!"

# Fedora/RHEL dependencies (requires LLVM 18)
deps-fedora:
	@echo "Installing dependencies for Fedora/RHEL..."
	@echo "Note: Fedora repos may not have LLVM 18. Ubuntu/Debian is recommended."
	sudo dnf install -y \
		clang \
		llvm18 \
		llvm18-devel \
		llvm18-static \
		flex \
		bison \
		gcc-c++ \
		zlib-devel \
		ncurses-devel \
		python3 \
		python3-pytest
	@echo "Dependencies installed successfully!"

help:
	@echo "Dana Compiler Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make            - Build the compiler"
	@echo "  make deps       - Install dependencies (Ubuntu/Debian/Fedora)"
	@echo "  make deps-ubuntu- Manual: Ubuntu/Debian dependencies (apt)"
	@echo "  make deps-fedora- Manual: Fedora/RHEL dependencies (dnf)"
	@echo "  make install    - Build and install to $(BINDIR) (may need sudo)"
	@echo "  make uninstall  - Remove installation"
	@echo "  make test       - Run test suite"
	@echo "  make clean      - Clean build artifacts"
	@echo ""
	@echo "Variables:"
	@echo "  PREFIX=$(PREFIX)  - Installation prefix"
	@echo "  BINDIR=$(BINDIR)  - Binary installation directory"
	@echo ""
	@echo "Quick Start:"
	@echo "  make deps                      # Install dependencies (auto-detects)"
	@echo "  sudo make install              # Build and install to /usr/local/bin"
	@echo "  make PREFIX=~/.local install   # Build and install to ~/.local/bin (no sudo)"
