#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../common/source_location.hpp"
#include "../symbol/symbol.hpp"
#include "../symbol/symbol_table.hpp"
#include "../symbol/sematype.hpp"
#include "diagnostics.hpp"

class ASTNode;

class SemContext {
public:
    SemContext(SymbolTable& st, Diagnostics& d);

	// accessors to symbol table and diagnostics
    SymbolTable& symtab();
    Diagnostics& diags();
	const SymbolTable& symtab() const;
	const Diagnostics& diags() const;
	bool hasErrors() const;
	void printDiagnostics() const;

	// api functions for managing scopes
    void openScope();
    void closeScope();
    std::size_t scopeDepth() const;

	// api function for looking up symbols
    LookupResult lookupSymbol(const std::string& name) const;
    LookupResult lookupLocalSymbol(const std::string& name) const;

    // api function for adding symbols to the current scope
	InsertResult declareSymbol(std::unique_ptr<Symbol> symbol, bool reportDuplicates = true);

	struct FunctionFrame {
		Symbol* symbol = nullptr;
		SemaTypePtr returnType;
		bool isProcedure = false;
		bool sawReturn = false;
	};

	struct LoopFrame {
		std::optional<std::string> label;
	};

	struct ParamInfo {
		std::string name;
		SourceLoc loc;
		SemaTypePtr type;
		Symbol::ParamPass passMode = Symbol::ParamPass::BY_VAL;
	};

	struct HeaderInfo {
		std::string name;
		SourceLoc loc;
		bool isProcedure = false;
		SemaTypePtr returnType;
		std::vector<ParamInfo> params;
	};

	void enterFunction(FunctionFrame frame);
	void leaveFunction();
	FunctionFrame* currentFunction();
	const FunctionFrame* currentFunction() const;

	void pushLoop(std::optional<std::string> label);
	void popLoop();
	bool inLoop() const;
	bool hasLoopLabel(const std::string& label) const;

	void setHeaderInfo(HeaderInfo info);
	std::optional<HeaderInfo> takeHeaderInfo();

private:
    SymbolTable& symtab_;
    Diagnostics& diags_;
	std::vector<FunctionFrame> functionStack_;
	std::vector<LoopFrame> loopStack_;
	std::optional<HeaderInfo> headerInfo_;
};
