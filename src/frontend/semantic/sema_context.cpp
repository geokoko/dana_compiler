#include "sema_context.hpp"

SemContext::SemContext(SymbolTable& st, Diagnostics& d)
    : symtab_(st), diags_(d) {}

SymbolTable& SemContext::symtab() {
    return symtab_;
}

const SymbolTable& SemContext::symtab() const {
    return symtab_;
}

Diagnostics& SemContext::diags() {
    return diags_;
}

const Diagnostics& SemContext::diags() const {
    return diags_;
}

bool SemContext::hasErrors() const {
	return diags_.hasErrors();
}

void SemContext::printDiagnostics() const {
	diags_.printAll();
}

void SemContext::openScope() {
    symtab_.openScope();
}

void SemContext::closeScope() {
    symtab_.closeScope();
}

std::size_t SemContext::scopeDepth() const {
    return symtab_.depth();
}

LookupResult SemContext::lookupSymbol(const std::string& name) const {
    return symtab_.lookup(name);
}

LookupResult SemContext::lookupLocalSymbol(const std::string& name) const {
    return symtab_.current().lookupLocal(name);
}

InsertResult SemContext::declareSymbol(std::unique_ptr<Symbol> symbol, bool reportDuplicates) {
    if (!symbol) {
        return {nullptr, false};
    }

    const std::string attemptedName = symbol->getName();
    const SourceLoc loc = symbol->getLocation();
    auto result = symtab_.declare(std::move(symbol));

    if (!result.inserted && reportDuplicates) {
        diags_.report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
                      loc, "symbol '" + attemptedName + "' already declared in this scope");
        if (result.symbol) {
            diags_.report(Diagnostics::Severity::Note, Diagnostics::Phase::Semantic,
                          result.symbol->getLocation(), "previous declaration is here");
        }
    }

    return result;
}

void SemContext::enterFunction(FunctionFrame frame) {
	functionStack_.push_back(std::move(frame));
}

void SemContext::leaveFunction() {
	if (!functionStack_.empty()) {
		functionStack_.pop_back();
	}
}

SemContext::FunctionFrame* SemContext::currentFunction() {
	return functionStack_.empty() ? nullptr : &functionStack_.back();
}

const SemContext::FunctionFrame* SemContext::currentFunction() const {
	return functionStack_.empty() ? nullptr : &functionStack_.back();
}

void SemContext::pushLoop(std::optional<std::string> label) {
	loopStack_.push_back(LoopFrame{std::move(label)});
}

void SemContext::popLoop() {
	if (!loopStack_.empty()) {
		loopStack_.pop_back();
	}
}

bool SemContext::inLoop() const {
	return !loopStack_.empty();
}

bool SemContext::hasLoopLabel(const std::string& label) const {
	for (auto it = loopStack_.rbegin(); it != loopStack_.rend(); ++it) {
		if (it->label && *it->label == label) {
			return true;
		}
	}
	return false;
}

void SemContext::setHeaderInfo(HeaderInfo info) {
	headerInfo_ = std::move(info);
}

std::optional<SemContext::HeaderInfo> SemContext::takeHeaderInfo() {
	auto tmp = std::move(headerInfo_);
	headerInfo_.reset();
	return tmp;
}


