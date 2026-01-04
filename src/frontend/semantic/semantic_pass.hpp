#ifndef SEMANTIC_PASS_HPP
#define SEMANTIC_PASS_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../ast/ast.hpp"
#include "../ast/ast_visitor.hpp"
#include "../symbol/sematype.hpp"
#include "../symbol/symbol.hpp"

class SemContext;

/// Helper struct to collect parameter information during semantic analysis
struct ParamInfo {
	std::string name;
	SourceLoc loc;
	SemaTypePtr type;
	Symbol::ParamPass passMode = Symbol::ParamPass::BY_VAL;
};

/// SemanticPass performs type checking and symbol resolution
class SemanticPass : public AstVisitor {
public:
	explicit SemanticPass(SemContext& context);

	// Node visitors
	void visit(Type& n) override;
	void visit(FParType& n) override;
	void visit(Program& n) override;
	void visit(FParDef& n) override;
	void visit(Header& n) override;
	void visit(VarDef& n) override;
	void visit(FuncDecl& n) override;
	void visit(FuncDef& n) override;
	void visit(Block& n) override;
	void visit(SkipStmt& n) override;
	void visit(ExitStmt& n) override;
	void visit(AssignStmt& n) override;
	void visit(ReturnStmt& n) override;
	void visit(ProcCall& n) override;
	void visit(BreakStmt& n) override;
	void visit(ContinueStmt& n) override;
	void visit(IfStmt& n) override;
	void visit(LoopStmt& n) override;
	void visit(IdLVal& n) override;
	void visit(StringLiteralLVal& n) override;
	void visit(IndexLVal& n) override;
	void visit(IntConst& n) override;
	void visit(CharConst& n) override;
	void visit(TrueConst& n) override;
	void visit(FalseConst& n) override;
	void visit(LValueExpr& n) override;
	void visit(ParenExpr& n) override;
	void visit(FuncCall& n) override;
	void visit(UnaryExpr& n) override;
	void visit(BinaryExpr& n) override;
	void visit(ExprCond& n) override;
	void visit(ParenCond& n) override;
	void visit(NotCond& n) override;
	void visit(BinaryCond& n) override;
	void visit(RelCond& n) override;

private:
	// Type utility helpers
	static bool isIntType(const SemaTypePtr& t);
	static bool isByteType(const SemaTypePtr& t);
	static bool isArrayType(const SemaTypePtr& t);
	static bool typesEqual(const SemaTypePtr& a, const SemaTypePtr& b);
	static bool arrayTypesCompatible(const ArrayType* actual, const ArrayType* expected);
	static bool typesCompatible(const SemaTypePtr& actual, const SemaTypePtr& expected);
	static SemaTypePtr scalarType(DataType dt);
	bool validateDimension(const std::optional<int>& dim, bool allowUnsized, const SourceLoc& loc);
	SemaTypePtr buildArrayType(const SourceLoc& loc, SemaTypePtr base, 
							const vec<std::optional<int>>& dims, bool allowUnsizedFirst);
	SemaTypePtr resolveType(const Type& node, bool allowUnsizedFirst = false);
	SemaTypePtr resolveParamType(const FParType& node, Symbol::ParamPass& pass);
	static std::string typeToString(const SemaTypePtr& type);

	// Semantic analysis helpers
	bool collectParams(const Header& header, std::vector<ParamInfo>& params);
	bool signaturesMatch(bool isProcedure, const SemaTypePtr& returnType,
					  const std::vector<ParamInfo>& params, const Symbol* symbol);
	bool checkArguments(const vec<up<Expr>>& args, const std::vector<ParamSymbol*>& params,
					 const std::string& callee, const SourceLoc& loc);

	SemContext& context_;
};

void runSemanticPass(Program& program, SemContext& context);

#endif // SEMANTIC_PASS_HPP
