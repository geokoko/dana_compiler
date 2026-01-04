# Dana Compiler - Top-level Makefile
# Usage:
#   make            - Build the compiler
#   make deps       - Install dependencies (detects distro automatically)
#   make install    - Build and install to /usr/local/bin (requires sudo)
#   make uninstall  - Remove from /usr/local/bin
#   make test       - Run test suite
#   make clean      - Clean build artifacts

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

# Detect Linux distribution
ifeq ($(shell uname -s),Linux)
  ifeq ($(shell test -f /etc/os-release && grep -q '^ID=ubuntu' /etc/os-release && echo ubuntu),ubuntu)
    DISTRO := ubuntu
  else ifeq ($(shell test -f /etc/os-release && grep -q '^ID_LIKE=.*debian' /etc/os-release && echo debian),debian)
    DISTRO := debian
  else ifeq ($(shell test -f /etc/os-release && grep -q '^ID=fedora' /etc/os-release && echo fedora),fedora)
    DISTRO := fedora
  else ifeq ($(shell test -f /etc/os-release && grep -q '^ID=rhel' /etc/os-release && echo rhel),rhel)
    DISTRO := fedora
  else
    DISTRO := unknown
  endif
else
  DISTRO := unknown
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

test: build
	pytest tests/ -v

clean:
	$(MAKE) -C src clean

# Detect and install dependencies automatically
deps:
	@echo "Detected system: $(DISTRO)"
	@if [ "$(DISTRO)" = "ubuntu" ] || [ "$(DISTRO)" = "debian" ]; then \
		$(MAKE) deps-ubuntu; \
	elif [ "$(DISTRO)" = "fedora" ]; then \
		$(MAKE) deps-fedora; \
	else \
		echo "ERROR: Unable to detect Linux distro. Please install dependencies manually."; \
		echo "See docs/building.md for manual installation instructions."; \
		exit 1; \
	fi

# Ubuntu/Debian dependencies
deps-ubuntu:
	@echo "Installing dependencies for Ubuntu/Debian..."
	sudo apt-get update
	sudo apt-get install -y \
		clang \
		llvm-18-dev \
		flex \
		bison \
		g++ \
		python3 \
		python3-pytest
	@echo "Dependencies installed successfully!"

# Fedora/RHEL dependencies
deps-fedora:
	@echo "Installing dependencies for Fedora/RHEL..."
	sudo dnf install -y \
		clang \
		llvm18-devel \
		flex \
		bison \
		gcc-c++ \
		python3 \
		python3-pytest
	@echo "Dependencies installed successfully!"

help:
	@echo "Dana Compiler Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make            - Build the compiler"
	@echo "  make deps       - Install dependencies (auto-detects distro)"
	@echo "  make deps-ubuntu- Install Ubuntu/Debian dependencies"
	@echo "  make deps-fedora- Install Fedora/RHEL dependencies"
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
