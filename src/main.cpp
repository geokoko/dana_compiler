#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "./frontend/ast/ast.hpp"
#include "./frontend/parser/parser.tab.hh"
#include "./frontend/common/diagnostics.hpp"
#include "./frontend/semantic/semantic.hpp"
#include "./frontend/semantic/sema_context.hpp"
#include "./frontend/symbol/symbol_table.hpp"
#include "./backend/codegen/codegen_context.hpp"
#include "./backend/codegen/codegen.hpp"
#include "./backend/optimizer/optimizer.h"
#include "./runtime/danalib.hpp"
#include "./runtime/lib_bitcode.hpp"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Error.h"
#include "llvm/IR/Verifier.h"

extern FILE* yyin;
namespace fs = std::filesystem;

struct PhaseTiming {
	std::string name;
	std::chrono::nanoseconds elapsed;
};

using Clock = std::chrono::steady_clock;

static void add_phase_timing(std::vector<PhaseTiming>& timings,
							 const std::string& name,
							 Clock::time_point start,
							 Clock::time_point end) {
	timings.push_back(PhaseTiming{name, std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)});
}

static void print_phase_timings(const std::vector<PhaseTiming>& timings) {
	if (timings.empty()) {
		return;
	}

	std::chrono::nanoseconds total{0};
	for (const auto& timing : timings) {
		total += timing.elapsed;
	}

	std::cerr << "Phase timings:\n";
	for (const auto& timing : timings) {
		const double ms = std::chrono::duration<double, std::milli>(timing.elapsed).count();
		std::cerr << "  " << timing.name << ": " << ms << " ms\n";
	}
	std::cerr << "  total: "
			  << std::chrono::duration<double, std::milli>(total).count()
			  << " ms\n";
}

static std::string change_extension(const std::string& path, const std::string& newExt) {
	std::size_t slashPos = path.find_last_of("/\\");
	std::size_t dotPos   = path.find_last_of('.');
	if (dotPos == std::string::npos || (slashPos != std::string::npos && dotPos < slashPos)) {
		return path + newExt;
	}
	return path.substr(0, dotPos) + newExt;
}

static std::string shell_quote(const std::string& value) {
	std::string quoted = "\"";
	for (char ch : value) {
		if (ch == '\\' || ch == '"' || ch == '$' || ch == '`') {
			quoted.push_back('\\');
		}
		quoted.push_back(ch);
	}
	quoted.push_back('"');
	return quoted;
}

static std::string make_temp_path(const std::string& stem, const std::string& ext) {
	static std::uint64_t counter = 0;
	const auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
		Clock::now().time_since_epoch()).count();
	const fs::path candidate = fs::temp_directory_path() /
		(stem + "." + std::to_string(now) + "." + std::to_string(counter++) + ext);
	return candidate.string();
}

static void remove_if_exists(const std::string& path) {
	if (path.empty()) {
		return;
	}
	std::error_code ec;
	fs::remove(path, ec);
}

static bool run_command(const std::string& cmd, std::chrono::nanoseconds* elapsed = nullptr) {
	auto start = Clock::now();
	int rc = std::system(cmd.c_str());
	auto end = Clock::now();
	if (elapsed) {
		*elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	}
	if (rc != 0) {
		std::cerr << "Command failed (" << rc << "): " << cmd << "\n";
		return false;
	}
	return true;
}

static void print_usage(const char* prog) {
	std::cerr << "Usage: " << prog
			  << " [--ast-tree] [--emit-ir] [--emit-asm] [--save-temps] [--time-phases] "
			  << "[-c] [-o output] [-O0|-O1|-O2|-O3] <file.dana>\n";
}

static bool link_runtime_bitcode(CodegenContext& codegenCtx) {
	auto& mainModule = codegenCtx.llvmModule();
	llvm::StringRef data(reinterpret_cast<const char*>(dana_runtime_bitcode), dana_runtime_bitcode_len);
	auto buffer = llvm::MemoryBuffer::getMemBufferCopy(data, "dana_runtime.bc");
	auto runtimeModuleOrErr = llvm::parseBitcodeFile(buffer->getMemBufferRef(), codegenCtx.llvmContext());
	if (!runtimeModuleOrErr) {
		std::cerr << "Error: failed to parse embedded runtime bitcode: "
		          << llvm::toString(runtimeModuleOrErr.takeError()) << "\n";
		return false;
	}

	auto runtimeModule = std::move(*runtimeModuleOrErr);

	if (mainModule.getTargetTriple().empty() && !runtimeModule->getTargetTriple().empty()) {
		mainModule.setTargetTriple(runtimeModule->getTargetTriple());
	} else if (runtimeModule->getTargetTriple().empty() && !mainModule.getTargetTriple().empty()) {
		runtimeModule->setTargetTriple(mainModule.getTargetTriple());
	}

	if (mainModule.getDataLayoutStr().empty() && !runtimeModule->getDataLayoutStr().empty()) {
		mainModule.setDataLayout(runtimeModule->getDataLayout());
	}
	runtimeModule->setDataLayout(mainModule.getDataLayout());

	if (llvm::Linker::linkModules(mainModule, std::move(runtimeModule))) {
		std::cerr << "Error: failed to link embedded runtime bitcode.\n";
		return false;
	}

	return true;
}

int main(int argc, char** argv) {
	bool want_tree = false;
	bool emit_ir = false;
	bool emit_asm = false;
	bool save_temps = false;
	bool time_phases = false;
	bool compile_only = false;
	const char* fname = nullptr;
	std::string outputPath;
	llvm::OptimizationLevel optLevel = llvm::OptimizationLevel::O0;
	std::string optFlag = "-O0";
	std::vector<PhaseTiming> phaseTimings;

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--ast-tree") {
			want_tree = true;
		} else if (arg == "--emit-ir") {
			emit_ir = true;
		} else if (arg == "--emit-asm") {
			emit_asm = true;
		} else if (arg == "--save-temps") {
			save_temps = true;
		} else if (arg == "--time-phases") {
			time_phases = true;
		} else if (arg == "-c") {
			compile_only = true;
		} else if (arg == "-o") {
			if (i + 1 >= argc) {
				print_usage(argv[0]);
				return 1;
			}
			outputPath = argv[++i];
		} else if (arg == "-O0") {
			optLevel = llvm::OptimizationLevel::O0;
			optFlag = "-O0";
		} else if (arg == "-O1") {
			optLevel = llvm::OptimizationLevel::O1;
			optFlag = "-O1";
		} else if (arg == "-O2") {
			optLevel = llvm::OptimizationLevel::O2;
			optFlag = "-O2";
		} else if (arg == "-O3") {
			optLevel = llvm::OptimizationLevel::O3;
			optFlag = "-O3";
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

	// Create global diagnostics before parsing so lexer/parser can use it
	Diagnostics diags;
	diags.setFilename(fname);
	dana::setGlobalDiagnostics(&diags);

	dana::parser::location_type loc;

	std::string filename = fname;
	loc.initialize();
	loc.begin.filename = &filename;
	loc.end.filename = &filename;

	up<Program> ast_root;
	dana::parser parser(ast_root);
	auto parseStart = Clock::now();
	int res = parser.parse();
	auto parseEnd = Clock::now();
	if (time_phases) {
		add_phase_timing(phaseTimings, "parse", parseStart, parseEnd);
	}

	std::fclose(yyin);

	if (res != 0) {
		diags.printAll();  // Print any accumulated lexer/parser errors
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
	SemContext semCtx(symtab, diags);
	declareBuiltins(semCtx);

	auto semanticStart = Clock::now();
	runSemanticPass(*ast_root, semCtx);
	runControlFlowPass(*ast_root, semCtx);
	auto semanticEnd = Clock::now();
	if (time_phases) {
		add_phase_timing(phaseTimings, "semantic", semanticStart, semanticEnd);
	}

	if (semCtx.hasErrors()) {
		semCtx.diags().printAll();
		std::cerr << "Semantic analysis finished with errors.\n";
		return 1;
	}

	CodegenContext codegenCtx("dana_module");
	genBuiltins(semCtx, codegenCtx);
	Codegen generate(codegenCtx);

	auto codegenStart = Clock::now();
	ast_root->accept(generate);
	auto codegenEnd = Clock::now();
	if (time_phases) {
		add_phase_timing(phaseTimings, "codegen", codegenStart, codegenEnd);
	}

	auto runtimeLinkStart = Clock::now();
	if (!link_runtime_bitcode(codegenCtx)) {
		return 1;
	}
	auto runtimeLinkEnd = Clock::now();
	if (time_phases) {
		add_phase_timing(phaseTimings, "link runtime bitcode", runtimeLinkStart, runtimeLinkEnd);
	}

	Optimizer optimizer;
	auto optimizeStart = Clock::now();
	optimizer.optimize(codegenCtx.llvmModule(), optLevel);
	auto optimizeEnd = Clock::now();
	if (time_phases) {
		add_phase_timing(phaseTimings, "optimize", optimizeStart, optimizeEnd);
	}

	if (emit_ir) {
		codegenCtx.llvmModule().print(llvm::outs(), nullptr);
		llvm::outs().flush();
	}

	if (llvm::verifyModule(codegenCtx.llvmModule(), &llvm::errs())) {
		std::cerr << "Error: generated LLVM IR is invalid.\n";
		return 1;
	}

	const std::string sourceStem = fs::path(filename).stem().string();
	const bool keepIr = emit_ir || save_temps;
	const bool keepObj = compile_only || save_temps;
	const std::string immPath = keepIr
		? change_extension(filename, ".ll")
		: make_temp_path(sourceStem, ".ll");
	const std::string asmPath = emit_asm
		? change_extension(filename, ".asm")
		: std::string{};
	const std::string objPath = compile_only
		? (!outputPath.empty() ? outputPath : change_extension(filename, ".o"))
		: (keepObj ? change_extension(filename, ".o") : make_temp_path(sourceStem, ".o"));
	const std::string execPath = compile_only
		? std::string{}
		: (!outputPath.empty() ? outputPath : "a.out");

	{
		auto irWriteStart = Clock::now();
		std::error_code EC;
		llvm::raw_fd_ostream immOut(immPath, EC, llvm::sys::fs::OF_Text);
		if (EC) {
			std::cerr << "Error: could not open " << immPath << " for writing: "
				<< EC.message() << "\n";
			return 1;
		}
		codegenCtx.llvmModule().print(immOut, nullptr);
		if (time_phases) {
			add_phase_timing(phaseTimings, "write .ll", irWriteStart, Clock::now());
		}
	}

	if (emit_asm) {
		// Use clang to compile LLVM IR to assembly
		std::string cmd = "clang " + optFlag + " -S " + shell_quote(immPath) +
			" -o " + shell_quote(asmPath);
		std::chrono::nanoseconds elapsed{0};
		if (!run_command(cmd, time_phases ? &elapsed : nullptr)) {
			if (!keepIr) {
				remove_if_exists(immPath);
			}
			std::cerr << "Failed to generate assembly (.asm).\n";
			return 1;
		}
		if (time_phases) {
			phaseTimings.push_back(PhaseTiming{"clang -S", elapsed});
		}
	}

	{
		// Use clang to compile LLVM IR to object file
		std::string cmd = "clang " + optFlag + " -c " + shell_quote(immPath) +
			" -o " + shell_quote(objPath);
		std::chrono::nanoseconds elapsed{0};
		if (!run_command(cmd, time_phases ? &elapsed : nullptr)) {
			if (!keepIr) {
				remove_if_exists(immPath);
			}
			if (!keepObj) {
				remove_if_exists(objPath);
			}
			std::cerr << "Failed to generate object file (.o).\n";
			return 1;
		}
		if (time_phases) {
			phaseTimings.push_back(PhaseTiming{"clang -c", elapsed});
		}
	}

	if (!compile_only) {
		// Use clang to link the generated object with system libraries.
		std::string linkCmd = "clang " + shell_quote(objPath) + " -o " + shell_quote(execPath);
		std::chrono::nanoseconds elapsed{0};
		if (!run_command(linkCmd, time_phases ? &elapsed : nullptr)) {
			if (!keepIr) {
				remove_if_exists(immPath);
			}
			if (!keepObj) {
				remove_if_exists(objPath);
			}
			std::cerr << "Failed to link executable.\n";
			return 1;
		}
		if (time_phases) {
			phaseTimings.push_back(PhaseTiming{"clang link", elapsed});
		}
	}

	if (!keepIr) {
		remove_if_exists(immPath);
	}
	if (!keepObj) {
		remove_if_exists(objPath);
	}

	std::cout << "Done.\n";
	std::cout << "Generated:\n";
	if (keepIr) {
		std::cout << "  IR:           " << immPath << "\n";
	}
	if (emit_asm) {
		std::cout << "  Assembly:     " << asmPath << "\n";
	}
	if (keepObj) {
		std::cout << "  Object:       " << objPath << "\n";
	}
	if (!compile_only) {
		std::cout << "  Executable:   " << execPath << "\n";
	}

	if (time_phases) {
		print_phase_timings(phaseTimings);
	}

	return 0;
}
