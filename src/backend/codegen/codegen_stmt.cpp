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
		stmt->agen(*this);
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
		rhs->agen(*this);
		rhsValue = value;
	}
	if (auto* lhs = n.left()) {
		lhs->agen(*this);
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
		expr->agen(*this);
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
	auto* function = genCtx.builder().GetInsertBlock() ? genCtx.builder().GetInsertBlock()->getParent() : nullptr;
	if (!function) {
		value = nullptr;
		return;
	}

	llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(genCtx.llvmContext(), "if.end", function);
	Block* elseBlockNode = n.elseBlock();
	llvm::BasicBlock* elseBB = elseBlockNode ? llvm::BasicBlock::Create(genCtx.llvmContext(), "if.else", function) : mergeBB;

	std::vector<std::pair<Cond*, Block*>> branches;
	branches.emplace_back(n.conditionExpr(), n.thenBlock());
	// Elif branches
	for (const auto& elif : n.elifs()) {
		branches.emplace_back(elif.first.get(), elif.second.get());
	}

	// lambda to ensure condition is boolean
	auto ensureBool = [&](llvm::Value* condVal) -> llvm::Value* {
		if (!condVal) {
			return llvm::ConstantInt::getFalse(genCtx.llvmContext());
		}
		if (condVal->getType()->isIntegerTy(1)) {
			return condVal;
		}
		if (condVal->getType()->isIntegerTy()) {
			auto* zero = llvm::ConstantInt::get(condVal->getType(), 0);
			return genCtx.builder().CreateICmpNE(condVal, zero, "if.cond");
		}
		return llvm::ConstantInt::getFalse(genCtx.llvmContext());
	};

	llvm::BasicBlock* nextCondBlock = genCtx.builder().GetInsertBlock();

	for (std::size_t i = 0; i < branches.size(); ++i) {
		Cond*  condNode  = branches[i].first;
		Block* bodyNode  = branches[i].second;
		bool   lastBranch = (i + 1 == branches.size());

		auto* trueBB  = llvm::BasicBlock::Create(genCtx.llvmContext(), "if.then" + std::to_string(i), function);
		llvm::BasicBlock* falseBB = nullptr;

		if (!lastBranch) {
			falseBB = llvm::BasicBlock::Create(
				genCtx.llvmContext(), "if.next" + std::to_string(i), function);
		} 
		else {
			falseBB = elseBB;
		}

		if (nextCondBlock) {
			genCtx.builder().SetInsertPoint(nextCondBlock);
		}
		value = nullptr;
		if (condNode) {
			condNode->agen(*this);
		}
		llvm::Value* condVal = ensureBool(value);
		value = nullptr;
		genCtx.builder().CreateCondBr(condVal, trueBB, falseBB);

		genCtx.builder().SetInsertPoint(trueBB);
		if (bodyNode) {
			bodyNode->agen(*this);
		}
		if (!trueBB->getTerminator()) {
			genCtx.builder().CreateBr(mergeBB);
		}

		if (!lastBranch) {
			nextCondBlock = falseBB;
		} 
		else if (!elseBlockNode) {
			nextCondBlock = mergeBB;
			genCtx.builder().SetInsertPoint(mergeBB);
		}
	}

	if (elseBlockNode) {
		genCtx.builder().SetInsertPoint(elseBB);
		elseBlockNode->agen(*this);
		if (!elseBB->getTerminator()) {
			genCtx.builder().CreateBr(mergeBB);
		}
	}

	genCtx.builder().SetInsertPoint(mergeBB);
	value = nullptr;
}

