#include "diagnostics.hpp"

#include <iostream>
#include <iomanip>

namespace {
	const char* severityToString(Diagnostics::Severity s) {
		switch (s) {
			case Diagnostics::Severity::Note: return "note";
			case Diagnostics::Severity::Warning: return "warning";
			case Diagnostics::Severity::Error: return "error";
		}
		return "unknown";
	}

	const char* phaseToString(Diagnostics::Phase p) {
		switch (p) {
			case Diagnostics::Phase::Lexing: return "lexical";
			case Diagnostics::Phase::Parsing: return "syntax";
			case Diagnostics::Phase::Semantic: return "semantic";
		}
		return "unknown";
	}

	// ANSI color codes for terminal output
	const char* severityColor(Diagnostics::Severity s) {
		switch (s) {
			case Diagnostics::Severity::Note: return "\033[1;36m";    // Bold cyan
			case Diagnostics::Severity::Warning: return "\033[1;35m"; // Bold magenta
			case Diagnostics::Severity::Error: return "\033[1;31m";   // Bold red
		}
		return "\033[0m";
	}

	const char* resetColor() {
		return "\033[0m";
	}

	const char* boldColor() {
		return "\033[1m";
	}
}

// Global diagnostics pointer
Diagnostics* g_diagnostics = nullptr;

void setGlobalDiagnostics(Diagnostics* diags) {
	g_diagnostics = diags;
}

Diagnostics* getGlobalDiagnostics() {
	return g_diagnostics;
}

void Diagnostics::setFilename(const std::string& filename) {
	filename_ = filename;
}

const std::string& Diagnostics::filename() const {
	return filename_;
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

int Diagnostics::errorCount() const {
	int count = 0;
	for (const auto& entry : messages_) {
		if (entry.severity == Severity::Error) {
			++count;
		}
	}
	return count;
}

const std::vector<Diagnostics::Entry>& Diagnostics::entries() const {
	return messages_;
}

void Diagnostics::printAll() const {
	for (const auto& entry : messages_) {
		// Format: filename:line:column: severity: message
		
		// Print location in bold white
		std::cerr << boldColor();
		if (!filename_.empty()) {
			std::cerr << filename_ << ":";
		}
		
		// Handle special builtin location
		if (entry.loc.line < 0) {
			std::cerr << "<builtin>: ";
		} else {
			std::cerr << entry.loc.line << ":" << entry.loc.col << ": ";
		}
		std::cerr << resetColor();
		
		// Print severity with color
		std::cerr << severityColor(entry.severity)
		          << severityToString(entry.severity) << ": "
		          << resetColor();
		
		// Print phase in parentheses (dimmed)
		std::cerr << "(" << phaseToString(entry.phase) << ") ";
		
		// Print the actual message
		std::cerr << entry.message << "\n";
	}
}

void Diagnostics::clearDiags() {
	messages_.clear();
}
