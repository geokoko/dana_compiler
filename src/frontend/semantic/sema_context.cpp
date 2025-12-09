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

std::unique_ptr<Symbol> SemContext::makeFunctionSymbol(const SemContext::HeaderInfo& info) {
	std::vector<SemaTypePtr> paramTypes;
	paramTypes.reserve(info.params.size());
	for (const auto& param : info.params) {
		paramTypes.push_back(param.type);
	}
	auto sig = makeFuncType(info.returnType, std::move(paramTypes));
	auto func = std::make_unique<FuncSymbol>(info.name, std::move(sig), info.isProcedure, info.loc);
	func->clearParams();
	for (const auto& param : info.params) {
		auto p = std::make_shared<ParamSymbol>(param.name, param.type, param.passMode, param.loc);
		p->setDefiningFunc(func.get());
		func->addParam(std::move(p));
	}
	return func;
}
