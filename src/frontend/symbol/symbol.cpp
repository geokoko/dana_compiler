#include "symbol.hpp"

#include <utility>

Symbol::Symbol(const std::string name, SymKind kind, SemaType* type, SourceLoc loc)
    : name_(name), kind_(kind), type_(type), loc_(loc) {}

const std::string& Symbol::name() const {
    return name_;
}

SymKind Symbol::getSymKind() const {
    return kind_;
}

const SemaType* Symbol::getType() const {
    return type_;
}

