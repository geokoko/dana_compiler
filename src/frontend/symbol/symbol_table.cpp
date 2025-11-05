#include "symbol_table.hpp"

#include <cassert>
#include <memory>

SymbolTable::SymbolTable() {
    pushScope();
}

void SymbolTable::pushScope() {
    Scope* parent = scopes_.empty() ? nullptr : scopes_.back().get();
    scopes_.push_back(std::make_unique<Scope>(parent));
}

void SymbolTable::popScope() {
    if (scopes_.empty()) {
        return;
    }
    scopes_.pop_back();
}

Scope& SymbolTable::current() {
    assert(!scopes_.empty());
    return *scopes_.back();
}

const Scope& SymbolTable::current() const {
    assert(!scopes_.empty());
    return *scopes_.back();
}

Symbol* SymbolTable::lookup(const std::string& id) const {
    return scopes_.empty() ? nullptr : scopes_.back()->lookup(id);
}
