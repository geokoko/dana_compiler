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

    SymbolTable& symtab();
    Diagnostics& diags();

    void openScope();
    void closeScope();
    std::size_t scopeDepth() const;

    Scope::LookupResult lookupSymbol(const std::string& name) const;
    Scope::LookupResult lookupLocalSymbol(const std::string& name) const;

    Scope::InsertResult declareSymbol(std::unique_ptr<Symbol> symbol, bool reportDuplicates = true);

private:
    SymbolTable& symtab_;
    Diagnostics& diags_;
};
