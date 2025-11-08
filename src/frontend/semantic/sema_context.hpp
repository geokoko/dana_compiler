#pragma once
#include "../symbol/symbol_table.hpp"
#include "diagnostics.hpp"
#include "../ast/ast.hpp"

/* SemContext object is a container for everything that is needed during semantic analysis.
 * It holds a reference to the symbol table, diagnostics object, and other context-specific
 * information such as the current function header (useful for return type checks) and loop context
 * (for break/continue statement validation).
 * It is passed to every sem() function of AST nodes, making use of encapsulation and avoiding
 * global variables.
 * */

class SemContext {
public:
	SemContext(SymbolTable& st, Diagnostics& d);

	SymbolTable& symtab();  // returns reference to symbol table
	Diagnostics& diags();   // returns reference to diagnostics
	
	const Header* currentHeader() const;  // returns pointer to current function header
	void setCurrentHeader(const Header* h); 
	void pushLoop(std::optional<std::string> label); // push a loop to the symbol table context
	void popLoop(std::optional<std::string> label);  // pop a loop from the symbol table context
	bool inLoop() const;						 // returns true if currently inside a loop										
	bool hasLoopLabel(const std::string& l) const; // returns true if a loop with the given label exists in the context

private:
	SymbolTable& symtab_;
	Diagnostics& diags_;
	const Header* currentHeader_ = nullptr;
	int loopDepth_ = 0;
	std::vector<std::string> loopLabels_;
};

