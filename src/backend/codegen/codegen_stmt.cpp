#include "codegen.hpp"

#include <vector>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

#include "../../frontend/ast/ast.hpp"

/* ========== Basic statements ========== */

void Codegen::visit(Block& n) {
	for (auto& stmt : n.statementsList()) {
		stmt->accept(*this);
	}
}

void Codegen::visit(SkipStmt& n) {
	(void)n;
	value = nullptr;
}

void Codegen::visit(ExitStmt& n) {
	(void)n;
	auto* curBB = genCtx.builder().GetInsertBlock();
	auto* parentFn = curBB ? curBB->getParent() : nullptr;
	if (!parentFn) {
		value = nullptr;
		return;
	}
	auto* retTy = parentFn->getReturnType();
	if (retTy->isVoidTy()) {
		genCtx.builder().CreateRetVoid();
	} else {
		genCtx.builder().CreateRet(llvm::ConstantInt::get(retTy, 0, true));
	}
	value = nullptr;
}

void Codegen::visit(AssignStmt& n) {
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

void Codegen::visit(ReturnStmt& n) {
	if (!genCtx.currentFunc()) {
		value = nullptr;
		return;
	}

	auto* fnSym = genCtx.currentFunc();
	// Use the actual LLVM return type, which reflects overrides like main -> i32
	auto* llvmFunc = genCtx.lookupFunction(fnSym);
	auto* retTy = llvmFunc ? llvmFunc->getReturnType() : genCtx.builder().getVoidTy();

	if (auto* expr = n.returnValue()) {
		expr->accept(*this);
		auto* v = value;
		genCtx.builder().CreateRet(v ? v : llvm::UndefValue::get(retTy));
	} else {
		if (retTy->isVoidTy()) {
			genCtx.builder().CreateRetVoid();
		} else {
			// Special case: if we are in main (or any forced i32 function) and returning void,
			// we should return 0.
			if (retTy->isIntegerTy()) {
				genCtx.builder().CreateRet(llvm::ConstantInt::get(retTy, 0));
			} else {
				genCtx.builder().CreateRet(llvm::UndefValue::get(retTy));
			}
		}
	}
	value = nullptr;
}

/* ========== If / else chain ========== */

void Codegen::visit(IfStmt& n) {
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
		
	llvm::BasicBlock* thenExit = genCtx.builder().GetInsertBlock();
	if (thenExit && !thenExit->getTerminator()) {
		genCtx.builder().CreateBr(mergeBB);
	}

		condBB = falseBB;
	}

	// Generate else block (if any)
	if (elseBlockNode) {
		value = nullptr;
		genCtx.builder().SetInsertPoint(elseBB);
		elseBlockNode->accept(*this);
		llvm::BasicBlock* elseExit = genCtx.builder().GetInsertBlock();
		if (elseExit && !elseExit->getTerminator()) {
			genCtx.builder().CreateBr(mergeBB);
		}
	}

	// Continue at merge block
	genCtx.builder().SetInsertPoint(mergeBB);
	value = nullptr;
}
