#pragma once

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "../../frontend/ast/ast.hpp"
#include "../../frontend/ast/ast_visitor.hpp"
#include "codegen_context.hpp"

// Concrete LLVM codegen visitor
class Codegen : public AstVisitor {
public:
	explicit Codegen(CodegenContext& ctx) : genCtx(ctx) { value = nullptr; }
	virtual ~Codegen() = default;

	// Result: 
	// For Expression nodes: the most recently evaluated computed data result
	// For L-value nodes: memory address
	// For Statements/voids: nullptr
	llvm::Value* value;

	// Top-level
	void visit(Type& n) override;
	void visit(FParType& n) override;
	void visit(Program& n) override;
	void visit(FParDef& n) override;
	void visit(Header& n) override;
	void visit(VarDef& n) override;
	void visit(FuncDecl& n) override;
	void visit(FuncDef& n) override;
	void visit(Block& n) override;

	// Statements
	void visit(SkipStmt& n) override;
	void visit(ExitStmt& n) override;
	void visit(AssignStmt& n) override;
	void visit(ReturnStmt& n) override;
	void visit(ProcCall& n) override;
	void visit(BreakStmt& n) override;
	void visit(ContinueStmt& n) override;
	void visit(IfStmt& n) override;
	void visit(LoopStmt& n) override;

	// L-values / expressions
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
	CodegenContext& genCtx;
	// Helper to generate parameter initialization code for function calls
	llvm::Value* makeCall(const FuncSymbol* calleeSym, const vec<up<Expr>>& args);

};
