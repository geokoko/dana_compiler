#include "codegen.hpp"

#include <algorithm>
#include <vector>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>

#include "../../frontend/ast/ast.hpp"
#include "../../frontend/symbol/symbol.hpp"

llvm::Value* LLVMCodegen::makeCall(const FuncSymbol* calleeSym, const std::vector<std::unique_ptr<Expr>>& args) {
	if (!calleeSym) {
		return nullptr;
	}

	llvm::Function* callee = genCtx.lookupFunction(calleeSym);
	if (!callee) {
		return nullptr;
	}

	auto* fnTy = callee->getFunctionType();
	std::vector<llvm::Value*> callArgs;
	callArgs.reserve(fnTy->getNumParams());
	bool needsStaticLink = calleeSym->definingFunc() != nullptr;
	unsigned paramIndex = 0;

	// Hidden static-link argument for nested routines
	if (needsStaticLink) {
		assert(fnTy->getNumParams() > 0 && "Nested routine missing static link parameter");

		llvm::Value* linkPtr = genCtx.currentFramePtr();
		const FuncSymbol* walker = genCtx.currentFunc();
		while (walker && walker != calleeSym->definingFunc() && linkPtr) {
			const auto* info = genCtx.getFrameInfo(walker);
			if (!info || !info->getFrameType()) {
				linkPtr = nullptr;
				break;
			}
			auto* frameTy    = info->getFrameType();
			auto* parentSlot = genCtx.builder().CreateStructGEP(frameTy, linkPtr, 0, "staticlink.slot");
			llvm::Type* parentPtrTy = frameTy->getElementType(0);
			linkPtr = genCtx.builder().CreateLoad(parentPtrTy, parentSlot, "staticlink.up");
			walker  = walker->definingFunc();
		}

		llvm::Type* staticLinkTy = fnTy->getParamType(paramIndex++);
		llvm::Value* staticLink  = nullptr;
		if (linkPtr) {
			if (linkPtr->getType() != staticLinkTy &&
				linkPtr->getType()->isPointerTy() &&
				staticLinkTy->isPointerTy()) {
				staticLink = genCtx.builder().CreateBitCast(linkPtr, staticLinkTy, "staticlink.cast");
			} 
			else {
				staticLink = linkPtr;
			}
		} 
		else {
			auto* ptrTy = llvm::cast<llvm::PointerType>(staticLinkTy);
			staticLink  = llvm::ConstantPointerNull::get(ptrTy);
		}
		callArgs.push_back(staticLink);
	}

	const auto& params  = calleeSym->getParams();
	const std::size_t paramCount = params.size();
	const std::size_t count      = std::min<std::size_t>(paramCount, args.size());

	auto getLValueNode = [](Expr* expr) -> Lval* {
		while (expr) {
			if (auto* lvExpr = dynamic_cast<LValueExpr*>(expr)) {
				return lvExpr->lvalue();
			}
			if (auto* paren = dynamic_cast<ParenExpr*>(expr)) {
				expr = paren->innerExpr();
				continue;
			}
			break;
		}
		return nullptr;
	};

	for (std::size_t i = 0; i < count; ++i) {
		auto* expr     = args[i].get();
		auto* paramSym = params[i].get();
		llvm::Type* paramTy = fnTy->getParamType(paramIndex++);
		llvm::Value* argVal = nullptr;

		if (expr) {
			const bool byRef = paramSym && paramSym->getPass() == Symbol::ParamPass::BY_REF;
			if (byRef) {
				if (auto* lvalNode = getLValueNode(expr)) {
					lvalNode->agen(*this);
					argVal = value;
				}
				if (!argVal) {
					expr->agen(*this);
					argVal = value;
				}
			} 
			else {
				expr->agen(*this);
				argVal = value;
			}
		}

		value = nullptr;

		if (!argVal) {
			argVal = llvm::UndefValue::get(paramTy);
		} 
		else if (argVal->getType() != paramTy &&
			argVal->getType()->isPointerTy() &&
			paramTy->isPointerTy()) {
			argVal = genCtx.builder().CreateBitCast(argVal, paramTy, "arg.cast");
		}
		callArgs.push_back(argVal);
	}

	for (std::size_t i = count; i < paramCount; ++i) {
		llvm::Type* paramTy = fnTy->getParamType(paramIndex++);
		callArgs.push_back(llvm::UndefValue::get(paramTy));
	}

	for (; paramIndex < fnTy->getNumParams(); ++paramIndex) {
		callArgs.push_back(llvm::UndefValue::get(fnTy->getParamType(paramIndex)));
	}

	return genCtx.builder().CreateCall(callee, callArgs);
}

void LLVMCodegen::gen(FuncCall& n) {
	const auto* calleeSym = static_cast<const FuncSymbol*>(n.symbol());
	if (!calleeSym) {
		value = nullptr;
		return;
	}
	value = makeCall(calleeSym, n.arguments());
}

void LLVMCodegen::gen(ProcCall& n) {
	value = nullptr;
	auto* calleeSym = static_cast<FuncSymbol*>(n.symbol());
	if (!calleeSym) {
		return;
	}
	makeCall(calleeSym, n.arguments());
}


