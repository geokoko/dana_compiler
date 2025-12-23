#pragma once
#include <string>
#include <vector>

#include "../common/source_location.hpp"

class Symbol;
class Scope;

struct InsertResult {
	Symbol* symbol = nullptr;
	bool inserted = false;
};

struct LookupResult {
	Symbol* symbol = nullptr;
	const Scope* owner = nullptr;
};

class Diagnostics {
public:
	enum class Severity {
		Note,
		Warning,
		Error
	};

	enum class Phase {
		Lexing,
		Parsing,
		Semantic
	};

	struct Entry {
		Severity severity;
		Phase phase;
		SourceLoc loc;
		std::string message;
	};

	void report(Severity severity, Phase phase, const SourceLoc& loc, const std::string& message);

	bool hasErrors() const;
	const std::vector<Entry>& entries() const;
	void printAll() const;
	void clearDiags();

private:
	std::vector<Entry> messages_;
};
