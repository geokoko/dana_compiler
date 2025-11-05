#include "scope.hpp"

#include <utility>

Scope::Scope(Scope* parent)
    : parent_(parent) {}

bool Scope::declare(std::unique_ptr<Symbol> symbol) {
    if (!symbol) {
        return false;
    }

    const std::string& id = symbol->name();
    auto [it, inserted] = table_.emplace(id, std::move(symbol));
    return inserted;
}

Symbol* Scope::lookupLocal(const std::string& id) const {
    auto it = table_.find(id);
    return it == table_.end() ? nullptr : it->second.get();
}

Symbol* Scope::lookup(const std::string& id) const {
    for (const Scope* scope = this; scope != nullptr; scope = scope->parent_) {
        if (auto* found = scope->lookupLocal(id)) {
            return found;
        }
    }
    return nullptr;
}

Scope* Scope::parent() const noexcept {
    return parent_;
}
