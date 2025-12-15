#include <cstdio>
#include <iostream>
#include <string>

#include "./frontend/ast/ast.hpp"
#include "./frontend/parser/parser.tab.hh"
#include "./frontend/semantic/diagnostics.hpp"
#include "./frontend/semantic/sema_context.hpp"
#include "./frontend/symbol/symbol_table.hpp"
#include "./backend/codegen/codegen_context.hpp"
#include "./backend/codegen/codegen.hpp"
#include "./runtime/danalib.hpp"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Verifier.h"

extern FILE* yyin;

static std::string change_extension(const std::string& path, const std::string& newExt) {
	std::size_t slashPos = path.find_last_of("/\\");
	std::size_t dotPos   = path.find_last_of('.');
	if (dotPos == std::string::npos || (slashPos != std::string::npos && dotPos < slashPos)) {
		return path + newExt;
	}
	return path.substr(0, dotPos) + newExt;
}

static bool run_command(const std::string& cmd) {
	int rc = std::system(cmd.c_str());
	if (rc != 0) {
		std::cerr << "Command failed (" << rc << "): " << cmd << "\n";
		return false;
	}
	return true;
}

static void print_usage(const char* prog) {
	std::cerr << "Usage: " << prog << " [--ast-tree] <file.dana>\n";
}

int main(int argc, char** argv) {
	bool want_tree = false;
	const char* fname = nullptr;

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--ast-tree") {
			want_tree = true;
		} else if (!fname) {
			fname = argv[i];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	}

	if (!fname) {
		print_usage(argv[0]);
		return 1;
	}

	yyin = std::fopen(fname, "r");
	if (!yyin) {
		std::cerr << "Error: could not open file " << fname << ".\n";
		return 1;
	}

	dana::parser::location_type loc;

	std::string filename = fname;
	loc.initialize();
	loc.begin.filename = &filename;
	loc.end.filename = &filename;

	up<Program> ast_root;
	dana::parser parser(ast_root);
	int res = parser.parse();

	std::fclose(yyin);

	if (res != 0) {
		std::cerr << "Parsing finished: failure (parse() returned " << res << ").\n";
		return res;
	}

	if (!ast_root) {
		std::cerr << "Parsing produced no AST.\n";
		return 1;
	}

	if (want_tree) {
		ast_root->print(std::cout);
	}

	SymbolTable symtab;
	Diagnostics diags;
	SemContext semCtx(symtab, diags);
	declareBuiltins(semCtx);

	ast_root->sem(semCtx);

	if (semCtx.hasErrors()) {
		semCtx.printDiagnostics();
		std::cerr << "Semantic analysis finished with errors.\n";
		return 1;
	}

	CodegenContext codegenCtx("dana_module");
	genBuiltins(semCtx, codegenCtx);
	LLVMCodegen generate(codegenCtx);
	ast_root->agen(generate);

	
	codegenCtx.llvmModule().print(llvm::outs(), nullptr);
	llvm::outs().flush();

	if (llvm::verifyModule(codegenCtx.llvmModule(), &llvm::errs())) {
		std::cerr << "Error: generated LLVM IR is invalid.\n";
		return 1;
	}

	std::string immPath = change_extension(filename, ".imm");
	{
		std::error_code EC;
		llvm::raw_fd_ostream immOut(immPath, EC, llvm::sys::fs::OF_Text);
		if (EC) {
			std::cerr << "Error: could not open " << immPath << " for writing: "
				<< EC.message() << "\n";
			return 1;
		}
		codegenCtx.llvmModule().print(immOut, nullptr);
	}
	std::string asmPath = change_extension(filename, ".asm");
	{
		std::string cmd = "llc -filetype=asm \"" + immPath + "\" -o \"" + asmPath + "\"";
		if (!run_command(cmd)) {
			std::cerr << "Failed to generate assembly (.asm).\n";
			return 1;
		}
	}

	std::string objPath = change_extension(filename, ".o");
	{
		std::string cmd = "llc -filetype=obj \"" + immPath + "\" -o \"" + objPath + "\"";
		if (!run_command(cmd)) {
			std::cerr << "Failed to generate object file (.o).\n";
			return 1;
		}
	}

	std::string execPath = "a.out";
	{
		// Use clang to link, and link against our Dana runtime object.
		std::string linkCmd = "clang \"" + objPath + "\" runtime/lib.o -o \"" + execPath + "\"";
		if (!run_command(linkCmd)) {
			std::cerr << "Failed to link executable.\n";
			return 1;
		}
	}

	std::cout << "Done.\n";
	std::cout << "Generated:\n"
		<< "  Intermediate: " << immPath << "\n"
		<< "  Assembly:     " << asmPath << "\n"
		<< "  Object:       " << objPath << "\n"
		<< "  Executable:   ./a.out\n";

	return 0;
}
