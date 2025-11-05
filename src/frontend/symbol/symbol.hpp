#pragma once

#include <string>

#include "../ast/ast.hpp"
#include "types.hpp"

enum class SymKind {
    VAR,
    PARAM_VAL,
    PARAM_REF,
    FUNC,
    PROC,
    DECL,
    STRING,
    CONST
};

class Symbol {
public:
    Symbol(std::string name, SymKind kind, SemaType type, const ASTNode* declaration = nullptr);

    const std::string& name() const noexcept;
    SymKind kind() const noexcept;
    const SemaType& type() const noexcept;
    const ASTNode* declaration() const noexcept;

private:
    std::string name_;
    SymKind kind_;
    SemaType type_;
    const ASTNode* declaration_;
};
