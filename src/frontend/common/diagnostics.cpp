#include "diagnostics.hpp"

#include <cstdarg>
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

	// Global diagnostics for lexer/parser
	Diagnostics* g_diagnostics = nullptr;
}

namespace dana {

void setGlobalDiagnostics(Diagnostics* diags) {
	g_diagnostics = diags;
}

Diagnostics* getGlobalDiagnostics() {
	return g_diagnostics;
}

} // namespace dana

void Diagnostics::setFilename(const std::string& filename) {
	filename_ = filename;
}

void Diagnostics::report(Severity severity, Phase phase, const SourceLoc& loc, const std::string& message) {
	messages_.push_back(Entry{severity, phase, loc, message});
	if (severity == Severity::Error) {
		++errorCount_;
	} else if (severity == Severity::Warning) {
		++warningCount_;
	}
}

void Diagnostics::report(Severity severity, int line, int col, const char* fmt, ...) {
	char buffer[512];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	
	SourceLoc loc{line, col};
	report(severity, Phase::Lexing, loc, std::string(buffer));
}

bool Diagnostics::hasErrors() const {
	return errorCount_ > 0;
}

std::size_t Diagnostics::errorCount() const {
	return errorCount_;
}

std::size_t Diagnostics::warningCount() const {
	return warningCount_;
}

const std::vector<Diagnostics::Entry>& Diagnostics::entries() const {
	return messages_;
}

void Diagnostics::printAll() const {
	for (const auto& entry : messages_) {
		if (!filename_.empty()) {
			std::cerr << filename_ << ':';
		}
		std::cerr << entry.loc.line << ':' << entry.loc.col << ": "
		          << toString(entry.severity) << ": "
		          << entry.message << '\n';
	}
}

void Diagnostics::clear() {
	messages_.clear();
	errorCount_ = 0;
	warningCount_ = 0;
}
