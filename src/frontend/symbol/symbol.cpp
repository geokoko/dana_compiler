#include "symbol.hpp"

#include <utility>

Symbol::Symbol(std::string name, SymKind kind, SemaTypePtr type, SourceLoc loc)
    : name_(std::move(name)),
      kind_(kind),
      type_(std::move(type)),
      loc_(loc) {}

const std::string& Symbol::getName() const {
    return name_;
}

Symbol::SymKind Symbol::getKind() const {
    return kind_;
}

const SemaTypePtr& Symbol::getType() const {
    return type_;
}

SourceLoc Symbol::getLocation() const {
    return loc_;
}

Symbol::ParamPass ParamSymbol::getPass() const {
    return pass_;
}

void FuncSymbol::addParam(std::shared_ptr<ParamSymbol> param) {
    if (param) {
        params_.push_back(std::move(param));
    }
}

const std::vector<std::shared_ptr<ParamSymbol>>& FuncSymbol::getParams() const {
    return params_;
}

const SemaTypePtr& FuncSymbol::getReturnType() const {
    return returnType_;
}

SemaTypePtr FuncSymbol::setReturnType(SemaTypePtr returnType) {
    SemaTypePtr previous = std::move(returnType_);
    returnType_ = std::move(returnType);
    return previous;
}

void ProcSymbol::addParam(std::shared_ptr<ParamSymbol> param) {
    if (param) {
        params_.push_back(std::move(param));
    }
}

const std::vector<std::shared_ptr<ParamSymbol>>& ProcSymbol::getParams() const {
    return params_;
}
