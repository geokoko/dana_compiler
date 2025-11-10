#include "symbol.hpp"

#include <utility>

Symbol::Symbol(std::string name, SymKind kind, SemaTypePtr type, SourceLoc loc)
	: name_(std::move(name)),
	  kind_(kind),
	  type_(std::move(type)),
	  loc_(loc) {}

const std::string& Symbol::name() const {
	return name_;
}

Symbol::SymKind Symbol::kind() const {
	return kind_;
}

const SemaTypePtr& Symbol::type() const {
	return type_;
}

const SourceLoc& Symbol::location() const {
	return loc_;
}

bool Symbol::isVariable() const {
	return kind_ == SymKind::Var;
}

bool Symbol::isParameter() const {
	return kind_ == SymKind::Param;
}

bool Symbol::isFunction() const {
	return kind_ == SymKind::Func || kind_ == SymKind::Proc;
}

bool Symbol::isProcedure() const {
	return kind_ == SymKind::Proc;
}

bool Symbol::isLabel() const {
	return kind_ == SymKind::Label;
}

void Symbol::markForwardDeclaration() {
	isForward_ = true;
}

void Symbol::markDefined() {
	isDefined_ = true;
}

bool Symbol::isForwardDeclaration() const {
	return isForward_;
}

bool Symbol::isDefined() const {
	return isDefined_;
}

bool Symbol::needsDefinition() const {
	return isForward_ && !isDefined_;
}

void Symbol::setParamPassing(SemaType::ParamPass pass) {
	paramPass_ = pass;
}

std::optional<SemaType::ParamPass> Symbol::paramPassing() const {
	return paramPass_;
}
