#include "codegen.hpp"

#include <vector>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

#include "../../frontend/ast/ast.hpp"

/* ========== Basic statements ========== */

void LLVMCodegen::gen(Block& n) {
	for (auto& stmt : n.statementsList()) {
		stmt->accept(*this);
	}
}

void LLVMCodegen::gen(SkipStmt& n) {
	(void)n;
	value = nullptr;
}

void LLVMCodegen::gen(ExitStmt& n) {
	(void)n;
	genCtx.builder().CreateRetVoid();
	value = nullptr;
}

void LLVMCodegen::gen(AssignStmt& n) {
	llvm::Value* rhsValue   = nullptr;
	llvm::Value* lhsAddress = nullptr;

	if (auto* rhs = n.right()) {
		rhs->accept(*this);
		rhsValue = value;
	}
	if (auto* lhs = n.left()) {
		lhs->accept(*this);
		lhsAddress = value;
	}

	if (rhsValue && lhsAddress) {
		genCtx.builder().CreateStore(rhsValue, lhsAddress);
	}
	value = nullptr;
}

void LLVMCodegen::gen(ReturnStmt& n) {
	if (!genCtx.currentFunc()) {
		value = nullptr;
		return;
	}

	auto* fnSym = genCtx.currentFunc();
	auto* sig   = static_cast<const FuncType*>(fnSym->getType().get());
	auto* retTy = sig && sig->returnType()
		? genCtx.getLLVMType(*sig->returnType())
		: genCtx.builder().getVoidTy();

	if (auto* expr = n.returnValue()) {
		expr->accept(*this);
		auto* v = value;
		genCtx.builder().CreateRet(v ? v : llvm::UndefValue::get(retTy));
	} else {
		if (retTy->isVoidTy()) {
			genCtx.builder().CreateRetVoid();
		} else {
			genCtx.builder().CreateRet(llvm::UndefValue::get(retTy));
		}
	}
	value = nullptr;
}

/* ========== If / else chain ========== */

void LLVMCodegen::gen(IfStmt& n) {
	// lambda to normalize condition to bool (i1)
	auto ensureBool = [&](llvm::Value* condVal) -> llvm::Value* {
		if (!condVal) {
			assert(false && "Condition value is null in if statement");
			return llvm::ConstantInt::getFalse(genCtx.llvmContext());
		}
		if (condVal->getType()->isIntegerTy(1)) {
			return condVal;
		}
		if (condVal->getType()->isIntegerTy()) {
			auto* zero = llvm::ConstantInt::get(condVal->getType(), 0);
			return genCtx.builder().CreateICmpNE(condVal, zero, "if.cond");
		}
		assert(false && "Unsupported condition type in if statement");
		return llvm::ConstantInt::getFalse(genCtx.llvmContext());
	};

	llvm::BasicBlock* curBB = genCtx.builder().GetInsertBlock();
	llvm::Function* function = curBB->getParent();

	llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(genCtx.llvmContext(), "if.end", function);
	Block* elseBlockNode = n.elseBlock();
	llvm::BasicBlock* elseBB = elseBlockNode ? llvm::BasicBlock::Create(genCtx.llvmContext(), "if.else", function) : mergeBB;

	// Collect all branches: if + elifs
	std::vector<std::pair<Cond*, Block*>> branches;
	branches.emplace_back(n.conditionExpr(), n.thenBlock());

	for (const auto& elif : n.elifs()) {
		branches.emplace_back(elif.first.get(), elif.second.get());
	}

	// Block where current condition is evaluated
	llvm::BasicBlock* condBB = curBB;

	for (std::size_t i = 0; i < branches.size(); ++i) {
		Cond*  condNode  = branches[i].first;
		Block* bodyNode  = branches[i].second;
		bool   lastBranch = (i == branches.size() - 1);

		auto* thenBB  = llvm::BasicBlock::Create(genCtx.llvmContext(), "if.then", function);
		llvm::BasicBlock* falseBB = nullptr;
		if (!lastBranch) {
			falseBB = llvm::BasicBlock::Create(genCtx.llvmContext(), "if.next" + std::to_string(i), function);
		} 
		else {
			falseBB = elseBB;
		}

		// Generate condition
		value = nullptr;
		genCtx.builder().SetInsertPoint(condBB);
		condNode->accept(*this);
		llvm::Value* condVal = ensureBool(value);
		value = nullptr;

		genCtx.builder().CreateCondBr(condVal, thenBB, falseBB);

		// Generate 'then' block
		genCtx.builder().SetInsertPoint(thenBB);
		bodyNode->accept(*this);
		
		if (!thenBB->getTerminator()) {
			genCtx.builder().CreateBr(mergeBB);
		}

		condBB = falseBB;
	}

	// Generate else block (if any)
	if (elseBlockNode) {
		value = nullptr;
		genCtx.builder().SetInsertPoint(elseBB);
		elseBlockNode->accept(*this);
		if (!elseBB->getTerminator()) {
			genCtx.builder().CreateBr(mergeBB);
		}
	}

	// Continue at merge block
	genCtx.builder().SetInsertPoint(mergeBB);
	value = nullptr;
}

