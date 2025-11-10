#pragma once
#include <string>
#include <vector>

#include "../common/source_location.hpp"

class Diagnostics {
public:
	enum class Severity {
		Info,
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
	void error(const SourceLoc& loc, const std::string& message);

	bool hasErrors() const;
	const std::vector<Entry>& entries() const;
	void printAll() const;

private:
	std::vector<Entry> messages_;
};
