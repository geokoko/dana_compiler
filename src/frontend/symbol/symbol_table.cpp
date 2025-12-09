#include "symbol_table.hpp"
#include <cassert>

SymbolTable::SymbolTable() {
	openScope();
}

void SymbolTable::openScope() {
	Scope* parent = scopes_.empty() ? nullptr : scopes_.back().get();
	scopes_.push_back(std::make_unique<Scope>(parent));
}

void SymbolTable::closeScope() {
	assert(!scopes_.empty());
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

LookupResult SymbolTable::lookup(const std::string& id) const {
	return scopes_.empty() ? LookupResult{} : scopes_.back()->lookup(id);
}

InsertResult SymbolTable::declare(std::unique_ptr<Symbol> symbol) {
	if (!symbol) {
		return {};
	}
	// Keep raw pointer before transferring ownership so we can insert into the current scope.
	Symbol* raw = symbol.get();
	// Store symbol to maintain ownership
	symbols_.push_back(std::move(symbol));
	// Pass raw pointer to Scope
	return current().declare(raw);
}

std::size_t SymbolTable::depth() const {
	return scopes_.size();
}
