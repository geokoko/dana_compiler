#pragma once

#include <optional>
#include <string>

#include "../common/source_location.hpp"
#include "sematype.hpp"

class Symbol {
public:
	enum class SymKind {
		Var,
		Param,
		Func,
		Proc,
		Label
	};

	Symbol(std::string name, SymKind kind, SemaTypePtr type, SourceLoc loc);
	~Symbol() = default;

	const std::string& name() const;
	SymKind kind() const;
	const SemaTypePtr& type() const;
	const SourceLoc& location() const;

	bool isVariable() const;
	bool isParameter() const;
	bool isFunction() const;
	bool isProcedure() const;
	bool isLabel() const;

	void markForwardDeclaration();
	void markDefined();
	bool isForwardDeclaration() const;
	bool isDefined() const;
	bool needsDefinition() const;

	void setParamPassing(SemaType::ParamPass pass);
	std::optional<SemaType::ParamPass> paramPassing() const;

private:
	std::string name_;
	SymKind kind_;
	SemaTypePtr type_;
	SourceLoc loc_;
	bool isForward_ = false;
	bool isDefined_ = false;
	std::optional<SemaType::ParamPass> paramPass_;
};
