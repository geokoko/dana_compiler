#include "scope.hpp"

#include <utility>

Scope::Scope(Scope* parent)
	: parent_(parent) {}

Scope::InsertResult Scope::declare(std::unique_ptr<Symbol> symbol) {
	if (!symbol) {
		return {};
	}

	const std::string& id = symbol->name();
	auto [it, inserted] = table_.emplace(id, std::move(symbol));
	return InsertResult{it->second.get(), inserted};
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

const std::unordered_map<std::string, std::unique_ptr<Symbol>>& Scope::symbols() const {
	return table_;
}
