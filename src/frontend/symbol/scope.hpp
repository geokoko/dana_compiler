#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "../common/diagnostics.hpp"
#include "symbol.hpp"

class Scope {
public:
	explicit Scope(Scope* parent = nullptr);

	InsertResult declare(Symbol* symbol);
	LookupResult lookupLocal(const std::string& id) const;
	LookupResult lookup(const std::string& id) const;
	Scope* parent() const noexcept;

	const std::unordered_map<std::string, Symbol*>& symbols() const;

private:
	std::unordered_map<std::string, Symbol*> table_;
	Scope* parent_ = nullptr;
};
