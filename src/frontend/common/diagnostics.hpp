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

	void setFilename(const std::string& filename);
	void report(Severity severity, Phase phase, const SourceLoc& loc, const std::string& message);
	
	// Convenience printf-style report for lexer/parser (defaults to Lexing phase)
	void report(Severity severity, int line, int col, const char* fmt, ...);

	bool hasErrors() const;
	std::size_t errorCount() const;
	std::size_t warningCount() const;
	const std::vector<Entry>& entries() const;
	void printAll() const;
	void clear();

private:
	std::vector<Entry> messages_;
	std::string filename_;
	std::size_t errorCount_ = 0;
	std::size_t warningCount_ = 0;
};

namespace dana {

// Global diagnostics pointer for lexer/parser integration
void setGlobalDiagnostics(Diagnostics* diags);
Diagnostics* getGlobalDiagnostics();

} // namespace dana
