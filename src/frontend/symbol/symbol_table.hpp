#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "scope.hpp"

class SymbolTable {
public:
	SymbolTable();

	void openScope();
	void closeScope();

	Scope& current();
	const Scope& current() const;

	Symbol* lookup(const std::string& id) const;
	Scope::InsertResult declare(std::unique_ptr<Symbol> symbol);

	std::size_t depth() const;

private:
	std::vector<std::unique_ptr<Scope>> scopes_;
};
