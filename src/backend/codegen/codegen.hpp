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
#include "codegen_context.hpp"

// Codegen visitor interface 
class Codegen {
public:
	virtual ~Codegen() = default;

	// Top-level
	virtual void gen(Type&) = 0;
	virtual void gen(FParType&) = 0;
	virtual void gen(Program&) = 0;
	virtual void gen(FParDef&) = 0;
	virtual void gen(Header&) = 0;
	virtual void gen(VarDef&) = 0;
	virtual void gen(FuncDecl&) = 0;
	virtual void gen(FuncDef&) = 0;
	virtual void gen(Block&) = 0;

	// Statements
	virtual void gen(SkipStmt&) = 0;
	virtual void gen(ExitStmt&) = 0;
	virtual void gen(AssignStmt&) = 0;
	virtual void gen(ReturnStmt&) = 0;
	virtual void gen(ProcCall&) = 0;
	virtual void gen(BreakStmt&) = 0;
	virtual void gen(ContinueStmt&) = 0;
	virtual void gen(IfStmt&) = 0;
	virtual void gen(LoopStmt&) = 0;

	// L-values / expressions
	virtual void gen(IdLVal&) = 0;
	virtual void gen(StringLiteralLVal&) = 0;
	virtual void gen(IndexLVal&) = 0;
	virtual void gen(IntConst&) = 0;
	virtual void gen(CharConst&) = 0;
	virtual void gen(TrueConst&) = 0;
	virtual void gen(FalseConst&) = 0;
	virtual void gen(LValueExpr&) = 0;
	virtual void gen(ParenExpr&) = 0;
	virtual void gen(FuncCall&) = 0;
	virtual void gen(UnaryExpr&) = 0;
	virtual void gen(BinaryExpr&) = 0;
	virtual void gen(ExprCond&) = 0;
	virtual void gen(ParenCond&) = 0;
	virtual void gen(NotCond&) = 0;
	virtual void gen(BinaryCond&) = 0;
	virtual void gen(RelCond&) = 0;
};

// Concrete LLVM codegen visitor
class LLVMCodegen : public Codegen {
public:
	explicit LLVMCodegen(CodegenContext& ctx) : genCtx(ctx) { value = nullptr; }

	// Top level
	void gen(Type& n) override;
	void gen(FParType& n) override;
	void gen(Program& n) override;
	void gen(FParDef& n) override;
	void gen(Header& n) override;
	void gen(VarDef& n) override;
	void gen(FuncDecl& n) override;
	void gen(FuncDef& n) override;
	void gen(Block& n) override;

	// Statements
	void gen(SkipStmt& n) override;
	void gen(ExitStmt& n) override;
	void gen(AssignStmt& n) override;
	void gen(ReturnStmt& n) override;
	void gen(ProcCall& n) override;
	void gen(BreakStmt& n) override;
	void gen(ContinueStmt& n) override;
	void gen(IfStmt& n) override;
	void gen(LoopStmt& n) override;

	// L-values / expressions
	void gen(IdLVal& n) override;
	void gen(StringLiteralLVal& n) override;
	void gen(IndexLVal& n) override;
	void gen(IntConst& n) override;
	void gen(CharConst& n) override;
	void gen(TrueConst& n) override;
	void gen(FalseConst& n) override;
	void gen(LValueExpr& n) override;
	void gen(ParenExpr& n) override;
	void gen(FuncCall& n) override;
	void gen(UnaryExpr& n) override;
	void gen(BinaryExpr& n) override;
	void gen(ExprCond& n) override;
	void gen(ParenCond& n) override;
	void gen(NotCond& n) override;
	void gen(BinaryCond& n) override;
	void gen(RelCond& n) override;

	// Result of the most recently evaluated expression is stored here. For voids, set to nullptr.
	llvm::Value* value; 

private:
	CodegenContext& genCtx;
	// Helper to generate parameter initialization code for function calls
	llvm::Value* parameterInit(const FuncSymbol* calleeSym, const vec<up<Expr>>& args);

};
