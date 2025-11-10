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

Symbol* SymbolTable::lookup(const std::string& id) const {
	return scopes_.empty() ? nullptr : scopes_.back()->lookup(id);
}

Scope::InsertResult SymbolTable::declare(std::unique_ptr<Symbol> symbol) {
	return current().declare(std::move(symbol));
}

std::size_t SymbolTable::depth() const {
	return scopes_.size();
}
