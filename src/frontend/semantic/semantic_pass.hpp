#pragma once

#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../ast/ast.hpp"
#include "../ast/ast_visitor.hpp"
#include "../common/source_location.hpp"
#include "../symbol/sematype.hpp"
#include "../symbol/symbol.hpp"
#include "sema_context.hpp"

class Program;
class SemContext;

/**
 * SemanticPass: Combined symbol declaration and type checking pass
 * 
 * Responsible for:
 * - Resolving type nodes
 * - Declaring variables in the symbol table
 * - Declaring functions and procedures (forward declarations and definitions)
 * - Setting up function frames and scopes
 * - Type checking expressions and statements
 * - Validating assignment compatibility
 * - Checking function/procedure call arguments
 * - Validating operator operand types
 * - Checking return statement types
 * - Setting type annotations on AST nodes
 */
class SemanticPass : public AstVisitor {
public:
	explicit SemanticPass(SemContext& context);

	// Type nodes
	void visit(Type& n) override;
	void visit(FParType& n) override;
	
	// Program structure
	void visit(Program& n) override;
	void visit(FParDef& n) override;
	void visit(Header& n) override;
	void visit(VarDef& n) override;
	void visit(FuncDecl& n) override;
	void visit(FuncDef& n) override;
	void visit(Block& n) override;
	
	// Statements
	void visit(SkipStmt&) override {}
	void visit(ExitStmt&) override {}
	void visit(AssignStmt& n) override;
	void visit(ReturnStmt& n) override;
	void visit(ProcCall& n) override;
	void visit(BreakStmt&) override {}
	void visit(ContinueStmt&) override {}
	void visit(IfStmt& n) override;
	void visit(LoopStmt& n) override;
	
	// L-values
	void visit(IdLVal& n) override;
	void visit(StringLiteralLVal& n) override;
	void visit(IndexLVal& n) override;
	
	// Expressions
	void visit(IntConst& n) override;
	void visit(TrueConst& n) override;
	void visit(FalseConst& n) override;
	void visit(CharConst& n) override;
	void visit(LValueExpr& n) override;
	void visit(ParenExpr& n) override;
	void visit(FuncCall& n) override;
	void visit(UnaryExpr& n) override;
	void visit(BinaryExpr& n) override;
	
	// Conditions
	void visit(ExprCond& n) override;
	void visit(ParenCond& n) override;
	void visit(NotCond& n) override;
	void visit(BinaryCond& n) override;
	void visit(RelCond& n) override;

private:
	// ----- Internal Data Structures -----

	struct ParamInfo {
		std::string name;
		SourceLoc loc;
		SemaTypePtr type;
		Symbol::ParamPass passMode = Symbol::ParamPass::BY_VAL;
	};

	struct HeaderInfo {
		std::string name;
		SourceLoc loc;
		bool isProcedure = false;
		SemaTypePtr returnType;
		std::vector<ParamInfo> params;
	};

	struct FunctionFrame {
		Symbol* symbol = nullptr;
		SemaTypePtr returnType;
		bool isProcedure = false;
	};

	// ----- Type Resolution Helpers -----
	
	// Type comparison helper
	static bool typesEqual(const SemaTypePtr& a, const SemaTypePtr& b);

	// Signature matching helper
	bool signaturesMatch(const HeaderInfo& info, const Symbol* symbol);

	// Converts DataType enum to semantic type
	static SemaTypePtr scalarType(DataType dt);

	// Array dimension validation
	static bool validateDimension(const std::optional<int>& dim, bool allowUnsized,
	                              const SourceLoc& loc, SemContext& context);

	// Builds array type from dimensions
	static SemaTypePtr buildArrayType(const SourceLoc& loc, SemaTypePtr base,
	                                  const vec<std::optional<int>>& dims,
	                                  bool allowUnsizedFirst, SemContext& context);

	// Resolves AST type node to semantic type
	static SemaTypePtr resolveType(const Type& node, SemContext& context,
	                               bool allowUnsizedFirst = false);

	// Resolves parameter type with pass mode
	static SemaTypePtr resolveParamType(const FParType& node, Symbol::ParamPass& pass,
	                                    SemContext& context);

	// Extracts header information from a Header AST node
	HeaderInfo extractHeaderInfo(Header& header);

	// ----- Type Checking Helpers -----

	// Type classification helpers
	static bool isIntType(const SemaTypePtr& t);
	static bool isByteType(const SemaTypePtr& t);
	static bool isArrayType(const SemaTypePtr& t);

	// Array type compatibility
	static bool arrayTypesCompatible(const ArrayType* actualArr, const ArrayType* expectedArr);
	static bool typesCompatible(const SemaTypePtr& actual, const SemaTypePtr& expected);

	// Diagnostic helper
	static std::string typeToString(const SemaTypePtr& type);

	// Argument checking
	bool checkArguments(const vec<up<Expr>>& args,
	                    const std::vector<ParamSymbol*>& params,
	                    const std::string& callee,
	                    const SourceLoc& loc);

	// ----- Function Frame Management -----

	void enterFunction(FunctionFrame frame);
	void leaveFunction();
	FunctionFrame* currentFunction();

	SemContext& context_;
	std::vector<FunctionFrame> functionStack_;
};

void runSemanticPass(Program& program, SemContext& context);
