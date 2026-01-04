#ifndef CONTROL_FLOW_HPP
#define CONTROL_FLOW_HPP

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "../ast/ast.hpp"
#include "../ast/ast_visitor.hpp"

class SemContext;

/// ControlFlowPass - Second pass of semantic analysis
///
/// Handles:
/// - Return statement validation (functions must return, procedures cannot)
/// - Exit statement validation (only valid in procedures)
/// - Break/continue validation (only valid inside loops)
/// - Loop label validation
/// - Reachability analysis (ensuring functions return on all paths)
class ControlFlowPass : public AstVisitor {
public:
	explicit ControlFlowPass(SemContext& context);

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
	struct FunctionInfo {
		std::string name;
		bool isProcedure = false;
		bool isEntrypoint = false;
	};

	// Reachability analysis helpers
	bool blockCanFallThrough(const Block* block);
	bool stmtCanFallThrough(const Stmt* stmt);
	bool blockHasBreakForLoop(const Block* block, const std::optional<std::string>& loopLabel, int depth);
	bool stmtHasBreakForLoop(const Stmt* stmt, const std::optional<std::string>& loopLabel, int depth);
	bool loopCanFallThrough(const LoopStmt* loop);

	SemContext& context_;
	std::vector<FunctionInfo> functionStack_;
	std::vector<std::optional<std::string>> loopStack_;
	std::unordered_set<std::string> activeLoopLabels_;
};

void runControlFlowPass(Program& program, SemContext& context);

#endif // CONTROL_FLOW_HPP
