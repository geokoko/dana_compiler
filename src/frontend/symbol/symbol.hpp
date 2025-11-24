#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../common/source_location.hpp"
#include "sematype.hpp"

class Symbol {
public:
	enum class SymKind {
		VAR,
		PARAM,
		FUNC
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
	bool isVariable() const;
	bool isParameter() const;

	void markForwardDeclaration();
	void markDefined();
	bool isForwardDeclaration() const;
	bool isDefined() const;

protected:
	Symbol(std::string name, SymKind kind, SemaTypePtr type, SourceLoc loc);

private:
	std::string name_;
	SymKind kind_;
	SemaTypePtr type_;
	SourceLoc loc_;
	bool isForward_ = false;
	bool isDefined_ = false;
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

// Note: A FuncSymbol's type is its signature type (return type + parameter types)
class FuncSymbol : public Symbol {
public:
	FuncSymbol(std::string name, SemaTypePtr sigType, bool isProc, SourceLoc loc):
		Symbol(std::move(name), SymKind::FUNC, std::move(sigType), loc), isProcedure_(isProc) {};
	
	void addParam(std::shared_ptr<ParamSymbol> param);
	const std::vector<std::shared_ptr<ParamSymbol>>& getParams() const;
	bool isProcedure () const;
	void clearParams();

private:
	std::vector<std::shared_ptr<ParamSymbol>> params_;
	bool isProcedure_;	
};
