#pragma once

#include <string>

#include "../ast/ast.hpp"
#include "sematype.hpp"

class Symbol {
public:
	enum class SymKind {
    	VAR,
    	PARAM,
    	FUNC,
    	PROC,
		LABEL
	};

    Symbol(const std::string& name, SymKind kind, SemaType* type, SourceLoc loc);
	~Symbol();

    const std::string& getName() const;
    SymKind getSymKind() const;
    const SemaType* getType() const;
    SourceLoc location() const;

	bool isVar() const;
	bool isFunc() const;
	bool isParam() const;
	bool isDecl() const;
	bool isProc() const;
	bool isLabel() const;

private:
    std::string name_;
    SymKind kind_;
    SemaType* type_;
    SourceLoc loc_;
};
