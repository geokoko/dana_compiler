#pragma once

#include <string>

#include "../common/source_location.hpp"
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

	enum class ParamPass {
		BY_VAL,
		BY_REF
	};

	virtual ~Symbol() = default;
	const std::string& getName() const;
	SymKind getKind() const;
	const SemaTypePtr& getType() const;
	SourceLoc getLocation() const;

protected:
	Symbol(std::string name, SymKind kind, SemaTypePtr type, SourceLoc loc);

private:
	std::string name_;
	SymKind kind_;
	SemaTypePtr type_;
	SourceLoc loc_;
};

class VarSymbol : public Symbol {
public:
	VarSymbol(std::string name, SemaTypePtr type, SourceLoc loc) :
		Symbol(std::move(name), SymKind::VAR, std::move(type), loc) {};
};

class ParamSymbol : public Symbol {
public:
	ParamSymbol(std::string name, SemaTypePtr type, ParamPass pass, SourceLoc loc) :
		Symbol(std::move(name), SymKind::PARAM, std::move(type), loc), pass_(pass) {};	
	ParamPass getPass() const;
private:
	ParamPass pass_;
};	

class FuncSymbol : public Symbol {
public:
	FuncSymbol(std::string name, SemaTypePtr returnType, SourceLoc loc):
		Symbol(std::move(name), SymKind::FUNC, std::move(returnType), loc) {};
	void addParam(std::shared_ptr<ParamSymbol> param);
	const std::vector<std::shared_ptr<ParamSymbol>>& getParams() const;
	const SemaTypePtr& getReturnType() const;
	SemaTypePtr setReturnType(SemaTypePtr returnType);

private:
	std::vector<std::shared_ptr<ParamSymbol>> params_;
	SemaTypePtr returnType_;
};

class ProcSymbol : public Symbol {
public:
	ProcSymbol(std::string name, SemaTypePtr procType, SourceLoc loc):
		Symbol(std::move(name), SymKind::PROC, std::move(procType), loc) {};
	void addParam(std::shared_ptr<ParamSymbol> param);
	const std::vector<std::shared_ptr<ParamSymbol>>& getParams() const;
private:
	std::vector<std::shared_ptr<ParamSymbol>> params_;
};

class LabelSymbol : public Symbol {
public:
	LabelSymbol(std::string name, SourceLoc loc):
		Symbol(std::move(name), SymKind::LABEL, nullptr, loc) {};

};
