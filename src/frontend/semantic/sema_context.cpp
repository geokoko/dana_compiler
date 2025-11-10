#include "sema_context.hpp"

#include <utility>

SemContext::SemContext(SymbolTable& st, Diagnostics& d)
	: symtab_(st), diags_(d) {}

SymbolTable& SemContext::symtab() {
	return symtab_;
}

Diagnostics& SemContext::diags() {
	return diags_;
}

const Header* SemContext::currentHeader() const {
	return currentHeader_;
}

void SemContext::setCurrentHeader(const Header* h) {
	currentHeader_ = h;
}

void SemContext::pushLoop(std::optional<std::string> label) {
	loopStack_.push_back(LoopInfo{std::move(label)});
}

void SemContext::popLoop(std::optional<std::string>) {
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

void SemContext::enterFunction(Symbol* symbol, SemaTypePtr returnType, bool isProcedure) {
	functionStack_.push_back(FunctionFrame{symbol, std::move(returnType), isProcedure});
}

void SemContext::leaveFunction() {
	if (!functionStack_.empty()) {
		functionStack_.pop_back();
	}
}

Symbol* SemContext::currentFunction() const {
	return functionStack_.empty() ? nullptr : functionStack_.back().symbol;
}

const SemaTypePtr& SemContext::currentReturnType() const {
	static SemaTypePtr nullType;
	return functionStack_.empty() ? nullType : functionStack_.back().returnType;
}

bool SemContext::currentFunctionIsProcedure() const {
	return !functionStack_.empty() && functionStack_.back().isProcedure;
}

bool SemContext::insideFunction() const {
	return !functionStack_.empty();
}

void SemContext::setHeaderInfo(HeaderInfo info) {
	pendingHeader_ = std::move(info);
}

std::optional<SemContext::HeaderInfo> SemContext::takeHeaderInfo() {
	if (!pendingHeader_) {
		return std::nullopt;
	}
	auto result = std::move(*pendingHeader_);
	pendingHeader_.reset();
	return result;
}

void SemContext::setNodeType(const ASTNode* node,
                             SemaTypePtr type,
                             bool isLValue,
                             bool assignable,
                             bool isConst) {
	nodeTypes_[node] = NodeTypeInfo{std::move(type), isLValue, assignable, isConst};
}

const SemContext::NodeTypeInfo* SemContext::nodeType(const ASTNode* node) const {
	auto it = nodeTypes_.find(node);
	return it == nodeTypes_.end() ? nullptr : &it->second;
}

SemaTypePtr SemContext::typeOf(const ASTNode* node) const {
	auto* info = nodeType(node);
	return info ? info->type : SemaTypePtr{};
}
