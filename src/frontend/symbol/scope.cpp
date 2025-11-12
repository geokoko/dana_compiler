#include "scope.hpp"

#include <utility>

Scope::Scope(Scope* parent)
	: parent_(parent) {}

// Declare symbol in current scope (aka insert into symbol table)
Scope::InsertResult Scope::declare(std::unique_ptr<Symbol> symbol) {
	if (!symbol) {
		return {};
	}

	const std::string& id = symbol->getName();
	auto [it, inserted] = table_.emplace(id, std::move(symbol));
	return InsertResult{it->second.get(), inserted};
}

// Lookup symbol in current scope only
Scope::LookupResult Scope::lookupLocal(const std::string& id) const {
	auto it = table_.find(id);
	if (it == table_.end()) {
		return {};
	}
	return LookupResult{it->second.get(), this};
}

// Lookup symbol in current scope and parent scopes
Scope::LookupResult Scope::lookup(const std::string& id) const {
	for (const Scope* scope = this; scope != nullptr; scope = scope->parent_) {
		// Look in current scope
		auto result = scope->lookupLocal(id);
		if (result.symbol) {
			return result;
		}
	}
	return {};
}

// Get parent scope
Scope* Scope::parent() const noexcept {
	return parent_;
}

// Get all symbols in the current scope
const std::unordered_map<std::string, std::unique_ptr<Symbol>>& Scope::symbols() const {
	return table_;
}
