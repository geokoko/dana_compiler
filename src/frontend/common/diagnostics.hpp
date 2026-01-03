#pragma once

#include <string>
#include <vector>

#include "source_location.hpp"

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

	// Set the source filename for better error messages
	void setFilename(const std::string& filename);
	const std::string& filename() const;

	void report(Severity severity, Phase phase, const SourceLoc& loc, const std::string& message);

	bool hasErrors() const;
	int errorCount() const;
	const std::vector<Entry>& entries() const;
	void printAll() const;
	void clearDiags();

private:
	std::vector<Entry> messages_;
	std::string filename_;
};

// Global diagnostics instance for lexer/parser use
extern Diagnostics* g_diagnostics;

void setGlobalDiagnostics(Diagnostics* diags);
Diagnostics* getGlobalDiagnostics();
