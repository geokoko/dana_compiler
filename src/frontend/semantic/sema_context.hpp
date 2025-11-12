#pragma once

#include <memory>
#include <string>

#include "../common/source_location.hpp"
#include "../symbol/scope.hpp"
#include "../symbol/symbol.hpp"
#include "../symbol/symbol_table.hpp"
#include "diagnostics.hpp"

class SemContext {
public:
    SemContext(SymbolTable& st, Diagnostics& d);

	// accessors to symbol table and diagnostics
    SymbolTable& symtab();
    Diagnostics& diags();

	// api functions for managing scopes
    void openScope();
    void closeScope();
    std::size_t scopeDepth() const;

	// api function for looking up symbols
    LookupResult lookupSymbol(const std::string& name) const;
    LookupResult lookupLocalSymbol(const std::string& name) const;

    // api function for adding symbols to the current scope
	InsertResult declareSymbol(std::unique_ptr<Symbol> symbol, bool reportDuplicates = true);

private:
    SymbolTable& symtab_;
    Diagnostics& diags_;
};
