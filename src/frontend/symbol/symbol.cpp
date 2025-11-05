#include "symbol.hpp"

#include <utility>

Symbol::Symbol(std::string name, SymKind kind, SemaType type, const ASTNode* declaration)
    : name_(std::move(name)), kind_(kind), type_(std::move(type)), declaration_(declaration) {}

const std::string& Symbol::name() const noexcept {
    return name_;
}

SymKind Symbol::kind() const noexcept {
    return kind_;
}

const SemaType& Symbol::type() const noexcept {
    return type_;
}

const ASTNode* Symbol::declaration() const noexcept {
    return declaration_;
}
