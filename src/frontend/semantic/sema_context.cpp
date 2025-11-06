#include "sema_context.hpp"

SemContext::SemContext(SymbolTable& st, Diagnostics& d) : symtab_(st), diags_(d) {}

SymbolTable& SemContext::symtab() { return symtab_; }
Diagnostics& SemContext::diags()  { return diags_; }

// For return / exit / break / continue rules:
const Header* SemContext::currentHeader() const { return currentHeader_; }
void SemContext::setCurrentHeader(const Header* h) { currentHeader_ = h; }

void SemContext::pushLoop(std::optional<std::string> label) {
	loopDepth_++;
	if (label) loopLabels_.push_back(*label);
}
void SemContext::popLoop(std::optional<std::string> label) {
	if (label && !loopLabels_.empty()) loopLabels_.pop_back();
	loopDepth_--;
}
bool SemContext::inLoop() const { return loopDepth_ > 0; }

bool SemContext::hasLoopLabel(const std::string& l) const {
	for (auto it = loopLabels_.rbegin(); it != loopLabels_.rend(); ++it)
		if (*it == l) return true;
	return false;
}

