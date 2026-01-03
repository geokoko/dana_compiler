#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "../ast/ast.hpp"
#include "../ast/ast_visitor.hpp"
#include "sema_context.hpp"

class Program;
class SemContext;

/**
 * ControlFlowPass: Second pass of semantic analysis
 * 
 * Responsible for:
 * - Validating return/exit statements are in appropriate contexts
 * - Checking that functions have return paths
 * - Validating break/continue statements are within loops
 * - Checking loop label validity
 * - Analyzing control flow for unreachable code detection
 * 
 * This pass assumes that symbol resolution and type checking have been completed.
 */
class ControlFlowPass : public AstVisitor {
public:
	explicit ControlFlowPass(SemContext& context);

	void visit(Type&) override {}
	void visit(FParType&) override {}
	void visit(Program& n) override;
	void visit(FParDef&) override {}
	void visit(Header&) override {}
	void visit(VarDef&) override {}
	void visit(FuncDecl&) override {}
	void visit(FuncDef& n) override;
	void visit(Block& n) override;
	void visit(SkipStmt&) override {}
	void visit(ExitStmt& n) override;
	void visit(AssignStmt&) override {}
	void visit(ReturnStmt& n) override;
	void visit(ProcCall&) override {}
	void visit(BreakStmt& n) override;
	void visit(ContinueStmt& n) override;
	void visit(IfStmt& n) override;
	void visit(LoopStmt& n) override;
	void visit(IdLVal&) override {}
	void visit(StringLiteralLVal&) override {}
	void visit(IndexLVal&) override {}
	void visit(IntConst&) override {}
	void visit(CharConst&) override {}
	void visit(TrueConst&) override {}
	void visit(FalseConst&) override {}
	void visit(LValueExpr&) override {}
	void visit(ParenExpr&) override {}
	void visit(FuncCall&) override {}
	void visit(UnaryExpr&) override {}
	void visit(BinaryExpr&) override {}
	void visit(ExprCond&) override {}
	void visit(ParenCond&) override {}
	void visit(NotCond&) override {}
	void visit(BinaryCond&) override {}
	void visit(RelCond&) override {}

private:
	struct FunctionInfo {
		std::string name;
		bool isProcedure = false;
		bool isEntrypoint = false;
	};

	bool blockCanFallThrough(const Block* block);
	bool stmtCanFallThrough(const Stmt* stmt);
	bool blockHasBreakForLoop(const Block* block,
		                     const std::optional<std::string>& loopLabel,
		                     int depth);
	bool stmtHasBreakForLoop(const Stmt* stmt,
		                    const std::optional<std::string>& loopLabel,
		                    int depth);
	bool loopCanFallThrough(const LoopStmt* loop);

	SemContext& context_;
	std::vector<FunctionInfo> functionStack_;
	std::vector<std::optional<std::string>> loopStack_;
	std::unordered_set<std::string> activeLoopLabels_;
};

void runControlFlowPass(Program& program, SemContext& context);
