#include "scope.hpp"

#include <utility>

Scope::Scope(Scope* parent)
	: parent_(parent) {}

// Declare symbol in current scope (aka insert into symbol table)
Scope::InsertResult Scope::declare(std::unique_ptr<Symbol> symbol) {
	if (!symbol) {
		return {};
	}

	const std::string& id = symbol->name();
	auto [it, inserted] = table_.emplace(id, std::move(symbol));
	return InsertResult{it->second.get(), inserted};
}

// Lookup symbol in current scope only
Symbol* Scope::lookupLocal(const std::string& id) const {
	auto it = table_.find(id);
	return it == table_.end() ? nullptr : it->second.get();
}

// Lookup symbol in current scope and parent scopes
Symbol* Scope::lookup(const std::string& id) const {
	for (const Scope* scope = this; scope != nullptr; scope = scope->parent_) {
		// Look in current scope
		if (auto* found = scope->lookupLocal(id)) {
			return found;
		}
	}
	return nullptr;
}

// Get parent scope
Scope* Scope::parent() const noexcept {
	return parent_;
}

// Get all symbols in the current scope
const std::unordered_map<std::string, std::unique_ptr<Symbol>>& Scope::symbols() const {
	return table_;
}
