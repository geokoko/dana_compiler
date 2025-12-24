#include "codegen.hpp"

#include <vector>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

#include "../../frontend/ast/ast.hpp"
#include "../../frontend/symbol/symbol.hpp"

/* ========== Constant expressions ========== */

void LLVMCodegen::gen(IntConst& n) {
	value = genCtx.builder().getInt32(n.getValue());
}

void LLVMCodegen::gen(CharConst& n) {
	value = genCtx.builder().getInt8(n.getValue());
}

void LLVMCodegen::gen(TrueConst& n) {
	(void)n;
	value = genCtx.builder().getInt8(1);
}

void LLVMCodegen::gen(FalseConst& n) {
	(void)n;
	value = genCtx.builder().getInt8(0);
}

/* ========== General expressions ========== */

void LLVMCodegen::gen(LValueExpr& n) {
	llvm::Value* addr = nullptr;
	if (auto* lv = n.lvalue()) {
		lv->accept(*this);
		addr = value;
	}
	if (!addr) {
		value = nullptr;
		return;
	}
	auto elemType = genCtx.getLLVMType(*n.type());
	value = genCtx.builder().CreateLoad(elemType, addr);
}

void LLVMCodegen::gen(ParenExpr& n) {
	if (auto* inner = n.innerExpr()) {
		inner->accept(*this);
	} else {
		value = nullptr;
	}
}

void LLVMCodegen::gen(UnaryExpr& n) {
	llvm::Value* operand = nullptr;
	if (auto* expr = n.operandExpr()) {
		expr->accept(*this);
		operand = value;
	}
	if (!operand) {
		value = nullptr;
		return;
	}

	switch (n.opKind()) {
		case UnOp::Plus:
			value = operand;
			break;
		case UnOp::Minus:
			value = genCtx.builder().CreateNeg(operand);
			break;
		case UnOp::Not:
			value = genCtx.builder().CreateNot(operand);
			break;
	}
}

void LLVMCodegen::gen(BinaryExpr& n) {
	llvm::Value* lhs = nullptr;
	llvm::Value* rhs = nullptr;
	if (auto* left = n.leftExpr()) {
		left->accept(*this);
		lhs = value;
	}
	if (auto* right = n.rightExpr()) {
		right->accept(*this);
		rhs = value;
	}
	if (!lhs || !rhs) {
		value = nullptr;
		return;
	}

	switch (n.opKind()) {
		case BinOp::Add:
			value = genCtx.builder().CreateAdd(lhs, rhs);
			break;
		case BinOp::Sub:
			value = genCtx.builder().CreateSub(lhs, rhs);
			break;
		case BinOp::Mul:
			value = genCtx.builder().CreateMul(lhs, rhs);
			break;
		case BinOp::Div:
			value = genCtx.builder().CreateSDiv(lhs, rhs);
			break;
		case BinOp::Mod:
			value = genCtx.builder().CreateSRem(lhs, rhs);
			break;
		case BinOp::AndBits:
			value = genCtx.builder().CreateAnd(lhs, rhs);
			break;
		case BinOp::OrBits:
			value = genCtx.builder().CreateOr(lhs, rhs);
			break;
		default:
			value = nullptr;
			break;
	}
}

/* ========== Lvalues ========== */

void LLVMCodegen::gen(IdLVal& n) {
	value = genCtx.lookupValue(n.symbol());
}

void LLVMCodegen::gen(StringLiteralLVal& n) {
	const std::string& literal = n.literal();
	auto* constStr = llvm::ConstantDataArray::getString(
		genCtx.llvmContext(), literal, true);
	auto* arrayTy = constStr->getType();

	static int strCounter = 0;
	std::string globalName = ".str." + std::to_string(strCounter++);

	auto* global = new llvm::GlobalVariable(
		genCtx.llvmModule(),
		arrayTy,
		/*isConstant=*/true,
		llvm::GlobalValue::PrivateLinkage,
		constStr,
		globalName);
	global->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
	value = global;
}

void LLVMCodegen::gen(IndexLVal& n) {
	llvm::Value* basePtr = nullptr;
	if (auto* base = n.baseExpr()) {
		base->accept(*this);
		basePtr = value;
	}
	if (!basePtr) {
		value = nullptr;
		return;
	}

	llvm::Value* indexVal = nullptr;
	if (auto* idx = n.indexExpr()) {
		idx->accept(*this);
		indexVal = value;
	}
	if (!indexVal) {
		value = nullptr;
		return;
	}

	// 32-bit index
	if (!indexVal->getType()->isIntegerTy(32)) {
		indexVal = genCtx.builder().CreateIntCast(indexVal, genCtx.builder().getInt32Ty(), true, "idx.cast");
	}

	auto* basePtrTy = basePtr->getType();
	if (!basePtrTy->isPointerTy()) {
		value = nullptr;
		return;
	}

	const auto* baseSema = n.baseExpr() ? n.baseExpr()->type().get() : nullptr;
	const auto* elemSema = n.type() ? n.type().get() : nullptr;
	llvm::Type* elemTy   = elemSema ? genCtx.getLLVMType(*elemSema) : nullptr;
	if (!elemTy) {
		value = nullptr;
		return;
	}

	if (baseSema && baseSema->getKind() == SemaType::TypeKind::ARRAY) {
		const auto& arrTy = static_cast<const ArrayType&>(*baseSema);
		if (arrTy.size()) {
			auto* arrLLVM = genCtx.getLLVMType(*baseSema, /*forParam=*/false);
			auto* zero    = genCtx.builder().getInt32(0);
			value = genCtx.builder().CreateInBoundsGEP(
				arrLLVM, basePtr, {zero, indexVal}, "idx.arr");
			return;
		}
		// unsized arrays decay to pointer-to-element
		value = genCtx.builder().CreateInBoundsGEP(elemTy, basePtr, indexVal, "idx.ptr");
		return;
	}

	// Fallback: treat basePtr as pointer to element type
	value = genCtx.builder().CreateInBoundsGEP(elemTy, basePtr, indexVal, "idx.elem");
}
