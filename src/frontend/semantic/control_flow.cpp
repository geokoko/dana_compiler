#include "control_flow.hpp"

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "../ast/ast.hpp"
#include "../ast/ast_visitor.hpp"
#include "sema_context.hpp"

// -----------------------------------------------------------------------------
// ControlFlowPass Constructor
// -----------------------------------------------------------------------------

ControlFlowPass::ControlFlowPass(SemContext& context) : context_(context) {}

// -----------------------------------------------------------------------------
// Reachability Analysis Helpers (private methods)
// -----------------------------------------------------------------------------

bool ControlFlowPass::blockCanFallThrough(const Block* block) {
	if (!block) {
		return true;
	}
	bool canFallThrough = true;
	for (const auto& stmt : block->statementsList()) {
		if (!canFallThrough) {
			break;
		}
		canFallThrough = stmtCanFallThrough(stmt.get());
	}
	return canFallThrough;
}

bool ControlFlowPass::stmtCanFallThrough(const Stmt* stmt) {
	if (!stmt) {
		return true;
	}
	switch (stmt->getKind()) {
		case Stmt::StmtKind::Return:
		case Stmt::StmtKind::Exit:
		case Stmt::StmtKind::Break:
		case Stmt::StmtKind::Continue:
			return false;
		case Stmt::StmtKind::If: {
			const auto* ifstmt = static_cast<const IfStmt*>(stmt);
			if (!ifstmt->elseBlock()) {
				return true;
			}
			bool canFallThrough = blockCanFallThrough(ifstmt->thenBlock());
			for (const auto& elif : ifstmt->elifs()) {
				canFallThrough = canFallThrough || blockCanFallThrough(elif.second.get());
			}
			canFallThrough = canFallThrough || blockCanFallThrough(ifstmt->elseBlock());
			return canFallThrough;
		}
		case Stmt::StmtKind::Loop:
			return loopCanFallThrough(static_cast<const LoopStmt*>(stmt));
		case Stmt::StmtKind::Skip:
		case Stmt::StmtKind::Assign:
		case Stmt::StmtKind::ProcCall:
			return true;
	}
	return true;
}

bool ControlFlowPass::blockHasBreakForLoop(const Block* block,
										   const std::optional<std::string>& loopLabel,
										   int depth) {
	if (!block) {
		return false;
	}
	for (const auto& stmt : block->statementsList()) {
		if (stmtHasBreakForLoop(stmt.get(), loopLabel, depth)) {
			return true;
		}
	}
	return false;
}

bool ControlFlowPass::stmtHasBreakForLoop(const Stmt* stmt,
										  const std::optional<std::string>& loopLabel,
										  int depth) {
	if (!stmt) {
		return false;
	}
	switch (stmt->getKind()) {
		case Stmt::StmtKind::Break: {
			const auto* brk = static_cast<const BreakStmt*>(stmt);
			if (brk->loopLabel()) {
				return loopLabel && *brk->loopLabel() == *loopLabel;
			}
			return depth == 0;
		}
		case Stmt::StmtKind::If: {
			const auto* ifstmt = static_cast<const IfStmt*>(stmt);
			if (blockHasBreakForLoop(ifstmt->thenBlock(), loopLabel, depth)) {
				return true;
			}
			for (const auto& elif : ifstmt->elifs()) {
				if (blockHasBreakForLoop(elif.second.get(), loopLabel, depth)) {
					return true;
				}
			}
			if (ifstmt->elseBlock() && blockHasBreakForLoop(ifstmt->elseBlock(), loopLabel, depth)) {
				return true;
			}
			return false;
		}
		case Stmt::StmtKind::Loop:
			return blockHasBreakForLoop(static_cast<const LoopStmt*>(stmt)->loopBody(), loopLabel, depth + 1);
		case Stmt::StmtKind::Skip:
		case Stmt::StmtKind::Exit:
		case Stmt::StmtKind::Assign:
		case Stmt::StmtKind::Return:
		case Stmt::StmtKind::ProcCall:
		case Stmt::StmtKind::Continue:
			return false;
	}
	return false;
}

bool ControlFlowPass::loopCanFallThrough(const LoopStmt* loop) {
	if (!loop) {
		return true;
	}
	return blockHasBreakForLoop(loop->loopBody(), loop->loopLabel(), 0);
}

// -----------------------------------------------------------------------------
// ControlFlowPass Visitor Implementations
// -----------------------------------------------------------------------------

void ControlFlowPass::visit(Type&) {}
void ControlFlowPass::visit(FParType&) {}

void ControlFlowPass::visit(Program& n) {
	if (auto* top = n.definition()) {
		top->accept(*this);
	}
}

void ControlFlowPass::visit(FParDef&) {}
void ControlFlowPass::visit(Header&) {}
void ControlFlowPass::visit(VarDef&) {}
void ControlFlowPass::visit(FuncDecl&) {}

void ControlFlowPass::visit(FuncDef& n) {
	auto* header = n.funcHeader();
	if (!header) {
		return;
	}
	FunctionInfo info;
	info.name = header->identifier();
	info.isProcedure = !header->returnType().has_value();
	info.isEntrypoint = n.isEntrypoint();
	functionStack_.push_back(info);

	for (auto& def : n.localDefs()) {
		if (def) {
			def->accept(*this);
		}
	}
	if (auto* body = n.funcBody()) {
		body->accept(*this);
	}

	if (!info.isProcedure && blockCanFallThrough(n.funcBody())) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "no path in function '" + info.name + "' returns a value");
	}

	functionStack_.pop_back();
}

void ControlFlowPass::visit(Block& n) {
	for (auto& stmt : n.statementsList()) {
		if (stmt) {
			stmt->accept(*this);
		}
	}
}

void ControlFlowPass::visit(SkipStmt&) {}

void ControlFlowPass::visit(ExitStmt& n) {
	if (functionStack_.empty()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "'exit' outside of procedure");
		return;
	}
	if (!functionStack_.back().isProcedure) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "'exit' allowed only inside procedures");
	}
}

void ControlFlowPass::visit(AssignStmt&) {}

void ControlFlowPass::visit(ReturnStmt& n) {
	if (functionStack_.empty()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "'return' outside of function");
		return;
	}
	if (functionStack_.back().isEntrypoint) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "return statement not allowed in main function");
		return;
	}
	if (functionStack_.back().isProcedure) {
		if (n.returnValue()) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						   n.loc, "procedures cannot return a value");
		}
		return;
	}
	if (!n.returnValue()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "functions must return a value");
	}
}

void ControlFlowPass::visit(ProcCall&) {}

void ControlFlowPass::visit(BreakStmt& n) {
	if (loopStack_.empty()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "'break' used outside of loop");
		return;
	}
	if (n.loopLabel() && activeLoopLabels_.find(*n.loopLabel()) == activeLoopLabels_.end()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "unknown loop label '" + *n.loopLabel() + "'");
	}
}

void ControlFlowPass::visit(ContinueStmt& n) {
	if (loopStack_.empty()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "'continue' used outside of loop");
		return;
	}
	if (n.loopLabel() && activeLoopLabels_.find(*n.loopLabel()) == activeLoopLabels_.end()) {
		context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						  n.loc, "unknown loop label '" + *n.loopLabel() + "'");
	}
}

void ControlFlowPass::visit(IfStmt& n) {
	if (auto* cond = n.conditionExpr()) {
		cond->accept(*this);
	}
	if (auto* thenBlock = n.thenBlock()) {
		thenBlock->accept(*this);
	}
	for (auto& elif : n.elifs()) {
		if (elif.first) {
			elif.first->accept(*this);
		}
		if (elif.second) {
			elif.second->accept(*this);
		}
	}
	if (auto* elseBlock = n.elseBlock()) {
		elseBlock->accept(*this);
	}
}

void ControlFlowPass::visit(LoopStmt& n) {
	const auto& label = n.loopLabel();
	bool insertedLabel = false;
	if (label) {
		auto result = activeLoopLabels_.insert(*label);
		insertedLabel = result.second;
		if (!insertedLabel) {
			context_.diags().report(Diagnostics::Severity::Error, Diagnostics::Phase::Semantic,
						   n.loc, "loop label '" + *label + "' already in use");
		}
	}
	loopStack_.push_back(label);

	if (auto* body = n.loopBody()) {
		body->accept(*this);
	}

	if (label && insertedLabel) {
		activeLoopLabels_.erase(*label);
	}
	loopStack_.pop_back();
}

void ControlFlowPass::visit(IdLVal&) {}
void ControlFlowPass::visit(StringLiteralLVal&) {}
void ControlFlowPass::visit(IndexLVal&) {}
void ControlFlowPass::visit(IntConst&) {}
void ControlFlowPass::visit(CharConst&) {}
void ControlFlowPass::visit(TrueConst&) {}
void ControlFlowPass::visit(FalseConst&) {}
void ControlFlowPass::visit(LValueExpr&) {}
void ControlFlowPass::visit(ParenExpr&) {}
void ControlFlowPass::visit(FuncCall&) {}
void ControlFlowPass::visit(UnaryExpr&) {}
void ControlFlowPass::visit(BinaryExpr&) {}
void ControlFlowPass::visit(ExprCond&) {}
void ControlFlowPass::visit(ParenCond&) {}
void ControlFlowPass::visit(NotCond&) {}
void ControlFlowPass::visit(BinaryCond&) {}
void ControlFlowPass::visit(RelCond&) {}

// -----------------------------------------------------------------------------
// Entry Point
// -----------------------------------------------------------------------------

void runControlFlowPass(Program& program, SemContext& context) {
	ControlFlowPass pass(context);
	program.accept(pass);
}
