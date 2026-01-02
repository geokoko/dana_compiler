#include "codegen.hpp"

#include <string>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "../../frontend/ast/ast.hpp"

/* ========== L-Values (value -> address) ========== */

void Codegen::visit(IdLVal& n) {
	auto* sym = n.symbol();
	if (!sym) {
		value = nullptr;
		return;
	}
	value = genCtx.lookupValue(sym);
}

void Codegen::visit(StringLiteralLVal& n) {
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

void Codegen::visit(IndexLVal& n) {
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

	// Now: 
	// basePtr -> address of the array in memory
	// indexVal -> evaluated expression inside brackets
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

/* ========== R-Values (value -> const data result) ========== */

void Codegen::visit(IntConst& n) {
	value = llvm::ConstantInt::get(genCtx.llvmContext(), llvm::APInt(32, n.getValue(), true));
}

void Codegen::visit(CharConst& n) {
	value = llvm::ConstantInt::get(genCtx.llvmContext(), llvm::APInt(8, n.getValue(), false));
}

void Codegen::visit(TrueConst& n) {
	(void)n;	
	value = llvm::ConstantInt::get(genCtx.llvmContext(), llvm::APInt(8, 1, false));
}

void Codegen::visit(FalseConst& n) {
	(void)n;
	value = llvm::ConstantInt::get(genCtx.llvmContext(), llvm::APInt(8, 0, false));
}


/* ========== Expressions -> (value -> data result) ========== */

void Codegen::visit(LValueExpr& n) {
	// Evaluate the LValue to get its address
	n.lvalue()->accept(*this);
	llvm::Value* addr = value;

	if (!addr) {
		return;
	}

	// Load the value from the address
	auto semaTy = n.type();
	llvm::Type* loadTy = genCtx.getLLVMType(*semaTy);

	value = genCtx.builder().CreateLoad(loadTy, addr, "load.val");
}

void Codegen::visit(ParenExpr& n) {
	if (auto* inner = n.innerExpr()) {
		inner->accept(*this);
	}
}

void Codegen::visit(UnaryExpr& n) {
	n.operandExpr()->accept(*this);
	llvm::Value* operand = value;

	if (!operand) return;

	switch (n.opKind()) {
		case UnOp::Plus:
			// No-op for integers
			value = operand;
			break;
		case UnOp::Minus:
			value = genCtx.builder().CreateNeg(operand, "neg");
			break;
		case UnOp::Not:
			{
				auto* zero = llvm::ConstantInt::get(operand->getType(), 0);
				auto* isZero = genCtx.builder().CreateICmpEQ(operand, zero, "not.cmp");
				// The result of ICmp is i1 (1-bit). We need to extend it back to i8 (byte).
				value = genCtx.builder().CreateZExt(isZero, operand->getType(), "not.res");
			}
			break;
	}
}

void Codegen::visit(BinaryExpr& n) {
	n.leftExpr()->accept(*this);
	llvm::Value* lhs = value;

	n.rightExpr()->accept(*this);
	llvm::Value* rhs = value;

	if (!lhs || !rhs) {
		value = nullptr;
		return;
	}

	switch (n.opKind()) {
		case BinOp::Add:
			value = genCtx.builder().CreateAdd(lhs, rhs, "add");
			break;
		case BinOp::Sub:
			value = genCtx.builder().CreateSub(lhs, rhs, "sub");
			break;
		case BinOp::Mul:
			value = genCtx.builder().CreateMul(lhs, rhs, "mul");
			break;
		case BinOp::Div:
			// Assuming signed division for 'int', unsigned for 'byte'
			if (lhs->getType()->isIntegerTy(8)) {
				value = genCtx.builder().CreateUDiv(lhs, rhs, "div.u");
			} else {
				value = genCtx.builder().CreateSDiv(lhs, rhs, "div.s");
			}
			break;
		case BinOp::Mod:
			if (lhs->getType()->isIntegerTy(8)) {
				value = genCtx.builder().CreateURem(lhs, rhs, "rem.u");
			} else {
				value = genCtx.builder().CreateSRem(lhs, rhs, "rem.s");
			}
			break;
		case BinOp::AndBits:
			value = genCtx.builder().CreateAnd(lhs, rhs, "and");
			break;
		case BinOp::OrBits:
			value = genCtx.builder().CreateOr(lhs, rhs, "or");
			break;
	}
}