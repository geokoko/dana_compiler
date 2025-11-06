#pragma once
#include "../symbol/symbol_table.hpp"
#include "diagnostics.hpp"
#include "../ast/ast.hpp"

class SemContext {
public:
	SemContext(SymbolTable& st, Diagnostics& d);

	SymbolTable& symtab();
	Diagnostics& diags();
	const Header* currentHeader() const;
	void setCurrentHeader(const Header* h);
	void pushLoop(std::optional<std::string> label);
	void popLoop(std::optional<std::string> label);
	bool inLoop() const;
	bool hasLoopLabel(const std::string& l) const;

private:
	SymbolTable& symtab_;
	Diagnostics& diags_;
	const Header* currentHeader_ = nullptr;
	int loopDepth_ = 0;
	std::vector<std::string> loopLabels_;
};

