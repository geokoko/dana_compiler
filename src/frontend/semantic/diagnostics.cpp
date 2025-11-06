#include "diagnostics.hpp"

bool Diagnostics::hasErrors() const {
	return !messages_.empty();
}

void Diagnostics::report(Severity severity, Phase phase, const std::string& message) {
	std::string prefix;
	switch (phase) {
		case Phase::LEXING: prefix += "[LEXING] "; break;
		case Phase::PARSING: prefix += "[PARSING] "; break;
		case Phase::SEMANTIC_ANALYSIS: prefix += "[SEMANTIC_ANALYSIS] "; break;
	}
	switch (severity) {
		case Severity::INFO: prefix = "[INFO] "; break;
		case Severity::WARNING: prefix = "[WARNING] "; break;
		case Severity::ERROR: prefix = "[ERROR] "; break;
	}
	messages_.push_back(prefix + message);
}

void Diagnostics::printAll() const {
	for (const auto& msg : messages_) {
		std::cout << msg << std::endl;
	}
}
