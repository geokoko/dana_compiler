#include "sema_context.hpp"

SemContext::SemContext(SymbolTable& st, Diagnostics& d)
    : symtab_(st), diags_(d) {}

SymbolTable& SemContext::symtab() {
    return symtab_;
}

Diagnostics& SemContext::diags() {
    return diags_;
}

void SemContext::openScope() {
    symtab_.openScope();
}

void SemContext::closeScope() {
    symtab_.closeScope();
}

std::size_t SemContext::scopeDepth() const {
    return symtab_.depth();
}

Symbol* SemContext::lookupSymbol(const std::string& name) const {
    return symtab_.lookup(name);
}

Symbol* SemContext::lookupLocalSymbol(const std::string& name) const {
    return symtab_.current().lookupLocal(name);
}

Symbol* SemContext::declareSymbol(std::unique_ptr<Symbol> symbol, bool reportDuplicates) {
    if (!symbol) {
        return nullptr;
    }

    const std::string attemptedName = symbol->name();
    const SourceLoc loc = symbol->location();
    auto result = symtab_.declare(std::move(symbol));

    if (!result.inserted && reportDuplicates) {
        diags_.error(loc, "symbol '" + attemptedName + "' already declared in this scope");
    }

    return result.symbol;
}

