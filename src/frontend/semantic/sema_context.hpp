#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../common/diagnostics.hpp"
#include "../symbol/symbol.hpp"
#include "../symbol/symbol_table.hpp"
#include "../symbol/sematype.hpp"

/**
 * SemContext - Lightweight wrapper for semantic analysis state.
 * 
 * Provides:
 * - Access to the symbol table for symbol lookup/declaration
 * - Access to diagnostics for error reporting
 * - Function stack for tracking the current function context (for return type checking)
 */
class SemContext {
public:
	SemContext(SymbolTable& st, Diagnostics& d);

	// -------------------------------------------------------------------------
	// Symbol table and diagnostics access
	// -------------------------------------------------------------------------
	SymbolTable& symtab();
	Diagnostics& diags();
	const SymbolTable& symtab() const;
	const Diagnostics& diags() const;
	bool hasErrors() const;

	// -------------------------------------------------------------------------
	// Scope management (delegates to symbol table)
	// -------------------------------------------------------------------------
	void openScope();
	void closeScope();
	std::size_t scopeDepth() const;

	// -------------------------------------------------------------------------
	// Symbol lookup and declaration
	// -------------------------------------------------------------------------
	LookupResult lookupSymbol(const std::string& name) const;
	LookupResult lookupLocalSymbol(const std::string& name) const;
	InsertResult declareSymbol(std::unique_ptr<Symbol> symbol, bool reportDuplicates = true);

	// -------------------------------------------------------------------------
	// Function context tracking (for return type validation)
	// -------------------------------------------------------------------------
	struct FunctionFrame {
		FuncSymbol* symbol = nullptr;
		SemaTypePtr returnType;
		bool isProcedure = false;
	};

	void enterFunction(FunctionFrame frame);
	void leaveFunction();
	FunctionFrame* currentFunction();
	const FunctionFrame* currentFunction() const;

private:
	SymbolTable& symtab_;
	Diagnostics& diags_;
	std::vector<FunctionFrame> functionStack_;
};
