#pragma once

#include <memory>
#include <string>
#include <vector>

#include "scope.hpp"

class SymbolTable {
public:
    SymbolTable();
	~SymbolTable();

    void openScope();
    void closeScope();

    Scope& current();
    const Scope& current() const;

    Symbol* lookup(const std::string& id) const;

private:
    std::vector<std::unique_ptr<Scope>> scopes_;
};
