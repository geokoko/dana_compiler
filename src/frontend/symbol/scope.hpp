#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "symbol.hpp"

class Scope {
public:
    explicit Scope(Scope* parent = nullptr);

    bool declare(std::unique_ptr<Symbol> symbol);
    Symbol* lookupLocal(const std::string& id) const;
    Symbol* lookup(const std::string& id) const;
    Scope* parent() const noexcept;

private:
    std::unordered_map<std::string, std::unique_ptr<Symbol>> table_;
    Scope* parent_;
};
