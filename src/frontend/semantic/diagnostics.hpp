#pragma once
#include <string>
#include <vector>

class Diagnostics {
private:
	enum class Severity {
		INFO,
		WARNING,
		ERROR
	};

	enum class Phase {
		LEXING,
		PARSING,
		SEMANTIC_ANALYSIS
	};

	std::vector<std::string> messages_;

public:
	Diagnostics() = default;
	~Diagnostics() = default;

	void report(Severity severity, Phase phase, const std::string& message); 
	bool hasErrors() const;
	void printAll() const;
};
