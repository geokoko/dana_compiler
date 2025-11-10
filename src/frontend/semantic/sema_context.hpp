#pragma once

#include <memory>
#include <string>

#include "../common/source_location.hpp"
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

    Symbol* lookupSymbol(const std::string& name) const;
    Symbol* lookupLocalSymbol(const std::string& name) const;

    Symbol* declareSymbol(std::unique_ptr<Symbol> symbol, bool reportDuplicates = true);


private:
    SymbolTable& symtab_;
    Diagnostics& diags_;
};
