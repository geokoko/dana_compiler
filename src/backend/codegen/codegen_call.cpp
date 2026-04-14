#include "codegen.hpp"

#include <algorithm>
#include <vector>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>

#include "../../frontend/ast/ast.hpp"
#include "../../frontend/symbol/symbol.hpp"

llvm::Value* Codegen::makeCall(const FuncSymbol* calleeSym, const std::vector<std::unique_ptr<Expr>>& args) {
	if (!calleeSym) return nullptr;

	llvm::Function* callee = genCtx.lookupFunction(calleeSym);
	if (!callee) return nullptr;

	auto* fnTy = callee->getFunctionType();
	std::vector<llvm::Value*> callArgs;
	callArgs.reserve(fnTy->getNumParams());
	bool needsStaticLink = calleeSym->definingFunc() != nullptr;
	unsigned paramIndex = 0;

	// Hidden static-link argument
	if (needsStaticLink) {
		llvm::Value* linkPtr = nullptr;
		const FuncSymbol* walker = genCtx.currentFunc();

		if (genCtx.currentFramePtr()) {
			linkPtr = genCtx.currentFramePtr();
		} else {
			linkPtr = genCtx.currentStaticLink();
			if (walker) walker = walker->definingFunc();
		}

		while (walker && walker != calleeSym->definingFunc() && linkPtr) {
			const auto* info = genCtx.getFrameInfo(walker);
			if (!info || !info->frameTy) {
				linkPtr = nullptr;
				break;
			}
			auto* frameTy    = info->frameTy;
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

	// Helper to get LValue node
	auto getLValueNode = [](Expr* expr) -> Lval* {
		while (expr) {
			switch (expr->getKind()) {
				case Expr::ExprKind::LValueExpr:
					return static_cast<LValueExpr*>(expr)->lvalue();
				case Expr::ExprKind::ParenExpr:
					expr = static_cast<ParenExpr*>(expr)->innerExpr();
					continue;
				case Expr::ExprKind::IntConst:
				case Expr::ExprKind::CharConst:
				case Expr::ExprKind::TrueConst:
				case Expr::ExprKind::FalseConst:
				case Expr::ExprKind::FuncCall:
				case Expr::ExprKind::UnaryExpr:
				case Expr::ExprKind::BinaryExpr:
				case Expr::ExprKind::ExprCond:
				case Expr::ExprKind::ParenCond:
				case Expr::ExprKind::NotCond:
				case Expr::ExprKind::BinaryCond:
				case Expr::ExprKind::RelCond:
					return nullptr;
			}
		}
		return nullptr;
	};

	for (std::size_t i = 0; i < count; ++i) {
		auto* expr     = args[i].get();
		auto* paramSym = params[i];
		llvm::Type* paramTy = fnTy->getParamType(paramIndex++);
		llvm::Value* argVal = nullptr;

		if (expr) {
			const bool byRef = paramSym && paramSym->getPass() == Symbol::ParamPass::BY_REF;
			if (byRef) {
				if (auto* lvalNode = getLValueNode(expr)) {
					lvalNode->accept(*this);
					argVal = value;
				}
				if (!argVal) {
					expr->accept(*this);
					argVal = value;
				}
			} 
			else {
				expr->accept(*this);
				argVal = value;
			}
		}

		if (!argVal) {
			argVal = llvm::Constant::getNullValue(paramTy);
		}
		value = nullptr;

		if (argVal->getType() != paramTy) {
			if (argVal->getType()->isPointerTy() && paramTy->isPointerTy()) {
				argVal = genCtx.builder().CreateBitCast(argVal, paramTy, "arg.cast");
			} else if (argVal->getType()->isIntegerTy() && paramTy->isIntegerTy()) {
				argVal = genCtx.builder().CreateIntCast(argVal, paramTy, true, "arg.intcast");
			}
		}
		callArgs.push_back(argVal);
	}

	// Fill remaining args with undef (optional params?)
	for (std::size_t i = count; i < paramCount; ++i) {
		llvm::Type* paramTy = fnTy->getParamType(paramIndex++);
		callArgs.push_back(llvm::UndefValue::get(paramTy));
	}

	for (; paramIndex < fnTy->getNumParams(); ++paramIndex) {
		callArgs.push_back(llvm::UndefValue::get(fnTy->getParamType(paramIndex)));
	}

	return genCtx.builder().CreateCall(callee, callArgs);
}

void Codegen::visit(FuncCall& n) {
	const auto* calleeSym = n.funcSymbol();
	if (!calleeSym) {
		value = nullptr;
		return;
	}
	value = makeCall(calleeSym, n.arguments());
}

void Codegen::visit(ProcCall& n) {
	value = nullptr;
	auto* calleeSym = n.funcSymbol();
	if (!calleeSym) {
		return;
	}
	makeCall(calleeSym, n.arguments());
}
