#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../common/source_location.hpp"
#include "../symbol/sematype.hpp"
#include "../symbol/symbol.hpp"
#include "../symbol/symbol_table.hpp"
#include "diagnostics.hpp"

class ASTNode;
class Header;

/* SemContext object is a container for everything that is needed during semantic analysis.
 * It holds a reference to the symbol table, diagnostics object, and other context-specific
 * information such as the current function header (useful for return type checks) and loop context
 * (for break/continue statement validation).
 * It is passed to every sem() function of AST nodes, making use of encapsulation and avoiding
 * global variables.
 * */

class SemContext {
public:
	struct ParamInfo {
		std::string name;
		SourceLoc loc;
		SemaTypePtr type;
		SemaType::ParamPass passMode = SemaType::ParamPass::BY_VAL;
	};

	struct HeaderInfo {
		std::string name;
		SourceLoc loc;
		SemaTypePtr funcType;
		Symbol::SymKind kind = Symbol::SymKind::Func;
		std::vector<ParamInfo> params;
	};

	struct NodeTypeInfo {
		SemaTypePtr type;
		bool isLValue = false;
		bool assignable = false;
		bool isConst = false;
	};

	SemContext(SymbolTable& st, Diagnostics& d);

	SymbolTable& symtab();  // returns reference to symbol table
	Diagnostics& diags();   // returns reference to diagnostics
	
	const Header* currentHeader() const;  // returns pointer to current function header
	void setCurrentHeader(const Header* h); 

	void pushLoop(std::optional<std::string> label); // push a loop to the symbol table context
	void popLoop(std::optional<std::string> label);  // pop a loop from the symbol table context
	bool inLoop() const;						 // returns true if currently inside a loop										
	bool hasLoopLabel(const std::string& l) const; // returns true if a loop with the given label exists in the context

	void enterFunction(Symbol* symbol, SemaTypePtr returnType, bool isProcedure);
	void leaveFunction();
	Symbol* currentFunction() const;
	const SemaTypePtr& currentReturnType() const;
	bool currentFunctionIsProcedure() const;
	bool insideFunction() const;

	void setHeaderInfo(HeaderInfo info);
	std::optional<HeaderInfo> takeHeaderInfo();

	void setNodeType(const ASTNode* node, SemaTypePtr type, bool isLValue = false, bool assignable = false, bool isConst = false);
	const NodeTypeInfo* nodeType(const ASTNode* node) const;
	SemaTypePtr typeOf(const ASTNode* node) const;

private:
	struct LoopInfo {
		std::optional<std::string> label;
	};

	struct FunctionFrame {
		Symbol* symbol = nullptr;
		SemaTypePtr returnType;
		bool isProcedure = false;
	};

	SymbolTable& symtab_;
	Diagnostics& diags_;

	const Header* currentHeader_ = nullptr;
	std::vector<LoopInfo> loopStack_;
	std::vector<FunctionFrame> functionStack_;
	std::optional<HeaderInfo> pendingHeader_;
	std::unordered_map<const ASTNode*, NodeTypeInfo> nodeTypes_;
};
