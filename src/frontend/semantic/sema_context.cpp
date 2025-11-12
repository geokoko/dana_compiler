#include "sema_context.hpp"
#include "../symbol/scope.hpp"

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

Scope::LookupResult SemContext::lookupSymbol(const std::string& name) const {
    return symtab_.lookup(name);
}

Scope::LookupResult SemContext::lookupLocalSymbol(const std::string& name) const {
    return symtab_.current().lookupLocal(name);
}

Scope::InsertResult SemContext::declareSymbol(std::unique_ptr<Symbol> symbol,
                                              bool reportDuplicates) {
    if (!symbol) {
        return {nullptr, false};
    }

    const std::string attemptedName = symbol->getName();
    const SourceLoc loc = symbol->getLocation();
    auto result = symtab_.declare(std::move(symbol));

    if (!result.inserted && reportDuplicates) {
        diags_.error(loc, "symbol '" + attemptedName + "' already declared in this scope");
    }

    return result;
}
