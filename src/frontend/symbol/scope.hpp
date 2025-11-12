#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "symbol.hpp"

class Scope {
public:
	struct InsertResult {
		Symbol* symbol = nullptr;
		bool inserted = false;
	};

	struct LookupResult {
		Symbol* symbol = nullptr;
		const Scope* owner = nullptr;
	};

	explicit Scope(Scope* parent = nullptr);

	InsertResult declare(std::unique_ptr<Symbol> symbol);
	LookupResult lookupLocal(const std::string& id) const;
	LookupResult lookup(const std::string& id) const;
	Scope* parent() const noexcept;

	const std::unordered_map<std::string, std::unique_ptr<Symbol>>& symbols() const;

private:
	std::unordered_map<std::string, std::unique_ptr<Symbol>> table_;
	Scope* parent_ = nullptr;
};
