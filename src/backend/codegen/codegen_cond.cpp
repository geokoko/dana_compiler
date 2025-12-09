#include "codegen.hpp"

#include <vector>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>

#include "../../frontend/ast/ast.hpp"

/* ========== Simple conditions ========== */

void LLVMCodegen::gen(ExprCond& n) {
	auto* expr = n.expression();
	if (!expr) {
		value = nullptr;
		return;
	}
	expr->agen(*this);
	auto* condVal = value;
	if (!condVal) {
		value = nullptr;
		return;
	}

	if (condVal->getType()->isIntegerTy(1)) {
		value = condVal;
		return;
	}
	if (condVal->getType()->isIntegerTy()) {
		auto* zero = llvm::ConstantInt::get(condVal->getType(), 0);
		value = genCtx.builder().CreateICmpNE(condVal, zero, "boolcast");
		return;
	}

	value = nullptr;
}

void LLVMCodegen::gen(ParenCond& n) {
	if (auto* inner = n.conditionExpr()) {
		inner->agen(*this);
	} 
	else {
		value = nullptr;
	}
}

void LLVMCodegen::gen(NotCond& n) {
	auto* inner = n.conditionExpr();
	if (!inner) {
		value = nullptr;
		return;
	}
	inner->agen(*this);
	auto* condVal = value;
	if (!condVal) {
		return;
	}

	if (!condVal->getType()->isIntegerTy(1)) {
		if (condVal->getType()->isIntegerTy()) {
			auto* zero = llvm::ConstantInt::get(condVal->getType(), 0);
			condVal = genCtx.builder().CreateICmpNE(condVal, zero, "boolcast");
		} 
		else {
			value = nullptr;
			return;
		}
	}
	value = genCtx.builder().CreateNot(condVal, "nottmp");
}

/* ========== Logical AND / OR ========== */

void LLVMCodegen::gen(BinaryCond& n) {
	auto* function = genCtx.builder().GetInsertBlock() ? genCtx.builder().GetInsertBlock()->getParent() : nullptr;
	if (!function) {
		value = nullptr;
		return;
	}

	auto ensureBool = [&](llvm::Value* condVal) -> llvm::Value* {
		if (!condVal) {
			return llvm::ConstantInt::getFalse(genCtx.llvmContext());
		}
		if (condVal->getType()->isIntegerTy(1)) {
			return condVal;
		}
		if (condVal->getType()->isIntegerTy()) {
			auto* zero = llvm::ConstantInt::get(condVal->getType(), 0);
			return genCtx.builder().CreateICmpNE(condVal, zero, "cond.bool");
		}
		return llvm::ConstantInt::getFalse(genCtx.llvmContext());
	};

	auto* rhsBB   = llvm::BasicBlock::Create(genCtx.llvmContext(), "cond.rhs", function);
	auto* mergeBB = llvm::BasicBlock::Create(genCtx.llvmContext(), "cond.end", function);

	if (auto* left = n.leftCond()) {
		left->agen(*this);
	}
	llvm::Value* lhsVal   = ensureBool(value);
	value                 = nullptr;
	llvm::BasicBlock* lhsBlock = genCtx.builder().GetInsertBlock();

	if (n.opKind() == LogicOp::And) {
		genCtx.builder().CreateCondBr(lhsVal, rhsBB, mergeBB);
		genCtx.builder().SetInsertPoint(rhsBB);

		if (auto* right = n.rightCond()) {
			right->agen(*this);
		}
		llvm::Value* rhsVal = ensureBool(value);
		genCtx.builder().CreateBr(mergeBB);
		value = nullptr;

		llvm::BasicBlock* rhsBlock = rhsBB;
		genCtx.builder().SetInsertPoint(mergeBB);
		auto* phi = genCtx.builder().CreatePHI(genCtx.builder().getInt1Ty(), 2, "and.tmp");
		phi->addIncoming(llvm::ConstantInt::getFalse(genCtx.llvmContext()), lhsBlock);
		phi->addIncoming(rhsVal, rhsBlock);
		value = phi;
	} 
	else if (n.opKind() == LogicOp::Or) {
		genCtx.builder().CreateCondBr(lhsVal, mergeBB, rhsBB);
		genCtx.builder().SetInsertPoint(rhsBB);

		if (auto* right = n.rightCond()) {
			right->agen(*this);
		}
		llvm::Value* rhsVal = ensureBool(value);
		genCtx.builder().CreateBr(mergeBB);
		value = nullptr;

		llvm::BasicBlock* rhsBlock = rhsBB;
		genCtx.builder().SetInsertPoint(mergeBB);
		auto* phi = genCtx.builder().CreatePHI(genCtx.builder().getInt1Ty(), 2, "or.tmp");
		phi->addIncoming(llvm::ConstantInt::getTrue(genCtx.llvmContext()), lhsBlock);
		phi->addIncoming(rhsVal, rhsBlock);
		value = phi;
	}
}

/* ========== Relational conditions ========== */

void LLVMCodegen::gen(RelCond& n) {
	llvm::Value* lhsVal = nullptr;
	if (auto* lhs = n.leftExpr()) {
		lhs->agen(*this);
		lhsVal = value;
	}
	llvm::Value* rhsVal = nullptr;
	if (auto* rhs = n.rightExpr()) {
		rhs->agen(*this);
		rhsVal = value;
	}
	if (!lhsVal || !rhsVal) {
		value = nullptr;
		return;
	}

	auto* lhsTy = lhsVal->getType();
	auto* rhsTy = rhsVal->getType();
	if (lhsTy != rhsTy) {
		if (lhsTy->isIntegerTy() && rhsTy->isIntegerTy()) {
			unsigned lhsBits    = lhsTy->getIntegerBitWidth();
			unsigned rhsBits    = rhsTy->getIntegerBitWidth();
			unsigned targetBits = std::max(lhsBits, rhsBits);

			if (lhsBits != targetBits) {
				lhsVal = genCtx.builder().CreateIntCast(lhsVal, llvm::IntegerType::get(genCtx.llvmContext(), targetBits), true, "lhs.cast");
			}
			if (rhsBits != targetBits) {
				rhsVal = genCtx.builder().CreateIntCast(rhsVal, llvm::IntegerType::get(genCtx.llvmContext(), targetBits), true, "rhs.cast");
			}
		} 
		else {
			value = nullptr;
			return;
		}
	}

	llvm::Value* cmp = nullptr;
	switch (n.opKind()) {
		case RelOp::Eq:
			cmp = genCtx.builder().CreateICmpEQ(lhsVal, rhsVal, "cmp.eq");
			break;
		case RelOp::Ne:
			cmp = genCtx.builder().CreateICmpNE(lhsVal, rhsVal, "cmp.ne");
			break;
		case RelOp::Lt:
			cmp = genCtx.builder().CreateICmpSLT(lhsVal, rhsVal, "cmp.lt");
			break;
		case RelOp::Le:
			cmp = genCtx.builder().CreateICmpSLE(lhsVal, rhsVal, "cmp.le");
			break;
		case RelOp::Gt:
			cmp = genCtx.builder().CreateICmpSGT(lhsVal, rhsVal, "cmp.gt");
			break;
		case RelOp::Ge:
			cmp = genCtx.builder().CreateICmpSGE(lhsVal, rhsVal, "cmp.ge");
			break;
	}
	value = cmp;
}

