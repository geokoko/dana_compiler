#include "diagnostics.hpp"

#include <iostream>

namespace {
	const char* toString(Diagnostics::Severity s) {
		switch (s) {
			case Diagnostics::Severity::Note: return "note";
			case Diagnostics::Severity::Warning: return "warning";
			case Diagnostics::Severity::Error: return "error";
		}
		return "?";
	}

	const char* toString(Diagnostics::Phase p) {
		switch (p) {
			case Diagnostics::Phase::Lexing: return "lexing";
			case Diagnostics::Phase::Parsing: return "parsing";
			case Diagnostics::Phase::Semantic: return "semantic";
		}
		return "?";
	}
}

void Diagnostics::report(Severity severity, Phase phase, const SourceLoc& loc, const std::string& message) {
	messages_.push_back(Entry{severity, phase, loc, message});
}

bool Diagnostics::hasErrors() const {
	for (const auto& entry : messages_) {
		if (entry.severity == Severity::Error) {
			return true;
		}
	}
	return false;
}

const std::vector<Diagnostics::Entry>& Diagnostics::entries() const {
	return messages_;
}

void Diagnostics::printAll() const {
	for (const auto& entry : messages_) {
		std::cout << entry.loc.line << ':' << entry.loc.col << " ["
		          << toString(entry.phase) << "] "
		          << toString(entry.severity) << ": "
		          << entry.message << '\n';
	}
}

void Diagnostics::clearDiags() {
	messages_.clear();
}
