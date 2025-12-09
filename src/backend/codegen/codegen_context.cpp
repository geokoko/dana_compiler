#include "codegen_context.hpp"

CodegenContext::CodegenContext(const std::string& moduleName) {
	ownedCtx_ = std::make_unique<llvm::LLVMContext>();
	module_ = std::make_unique<llvm::Module>(moduleName, *ownedCtx_);
	builder_ = std::make_unique<llvm::IRBuilder<>>(*ownedCtx_);
}

void CodegenContext::FrameInfo::setFrameType(llvm::StructType* frameType) {
	frameTy = frameType;
}

llvm::StructType* CodegenContext::FrameInfo::getFrameType() {
	return frameTy;
}

llvm::StructType* CodegenContext::FrameInfo::getFrameType() const {
	return frameTy;
}

void CodegenContext::FrameInfo::captureVar(const Symbol* sym, std::size_t index) {
	if (!sym) return;
	capturedVars[sym] = index;
}

std::optional<std::size_t> CodegenContext::FrameInfo::getCapturedVarIndex(const Symbol* sym) const {
	if (!sym) {
		return std::nullopt;
	}
	auto it = capturedVars.find(sym);
	if (it == capturedVars.end()) {
		return std::nullopt;
	}
	return it->second;
}

llvm::Function* CodegenContext::lookupFunction(const FuncSymbol* sym) const {
	auto it = functionMap_.find(sym);
	return it == functionMap_.end() ? nullptr : it->second;
}

llvm::Function* CodegenContext::getLLVMFunction(const FuncSymbol* fn) {
	auto it = functionMap_.find(fn);
	return it == functionMap_.end() ? nullptr : it->second;
}

void CodegenContext::bindFunction(const FuncSymbol* sym, llvm::Function* fn) {
	if (!sym) return;
	functionMap_[sym] = fn;
}

void CodegenContext::FrameInfo::pushLoop(llvm::BasicBlock* breakBB, llvm::BasicBlock* continueBB) {
	breakTargets.push_back(breakBB);
	continueTargets.push_back(continueBB);
}

void CodegenContext::FrameInfo::popLoop() {
	if (!breakTargets.empty()) {
		breakTargets.pop_back();
	}
	if (!continueTargets.empty()) {
		continueTargets.pop_back();
	}
}

llvm::BasicBlock* CodegenContext::FrameInfo::currentBreakTarget() const {
	return breakTargets.empty() ? nullptr : breakTargets.back();
}

llvm::BasicBlock* CodegenContext::FrameInfo::currentContinueTarget() const {
	return continueTargets.empty() ? nullptr : continueTargets.back();
}

llvm::Type* CodegenContext::getLLVMType(const SemaType& ty, bool forParam) {
	switch (ty.getKind()) {
		case SemaType::TypeKind::INT:
			return llvm::Type::getInt32Ty(llvmContext());
		case SemaType::TypeKind::BYTE:
			return llvm::Type::getInt8Ty(llvmContext());
		case SemaType::TypeKind::VOID:
			return llvm::Type::getVoidTy(llvmContext());
		case SemaType::TypeKind::ARRAY: {
			const auto& arrTy = static_cast<const ArrayType&>(ty);
			auto elem = arrTy.elementType();

			if (!elem) {
				return nullptr;
			}

			auto* elemTy = getLLVMType(*elem, /*forParam=*/false);

			// Unsized arrays decay to opaque pointers (primarily for parameters)
			if (!arrTy.size()) {
				return llvm::PointerType::get(llvmContext(), 0);
			}

			auto* llvmArrTy = llvm::ArrayType::get(elemTy, *arrTy.size());
			return forParam ? static_cast<llvm::Type*>(llvm::PointerType::get(llvmContext(), 0)) 
							: static_cast<llvm::Type*>(llvmArrTy);
		}
		case SemaType::TypeKind::FUNC: {
			forParam = false; // function types never decay
			const auto& fnTy = static_cast<const FuncType&>(ty);
			std::vector<llvm::Type*> paramTypes;
			paramTypes.reserve(fnTy.params().size());

			for (const auto& p : fnTy.params()) {
				if (p) {
					paramTypes.push_back(getLLVMType(*p, /*forParam=*/true));
				}
			}

			auto* retTy = fnTy.returnType() ? getLLVMType(*fnTy.returnType(), /*forParam=*/false)
				: llvm::Type::getVoidTy(llvmContext());
			auto* llvmFnTy = llvm::FunctionType::get(retTy, paramTypes, /*isVarArg=*/false);
			return llvmFnTy;
		}
	}
	return nullptr;
}

CodegenContext::FrameInfo* CodegenContext::createFrameInfo(const FuncSymbol* fn) {
	if (!fn) {
		return nullptr;
	}
	auto [it, inserted] = frameMap_.try_emplace(fn);
	if (inserted || !it->second) {
		it->second = std::make_unique<FrameInfo>();
	}
	return it->second.get();
}

CodegenContext::FrameInfo* CodegenContext::getFrameInfo(const FuncSymbol* f) {
	auto it = frameMap_.find(f);
	return it != frameMap_.end() ? it->second.get() : nullptr;
}

const CodegenContext::FrameInfo* CodegenContext::getFrameInfo(const FuncSymbol* fn) const {
	auto it = frameMap_.find(fn);
	return it == frameMap_.end() ? nullptr : it->second.get();
}

llvm::Value* CodegenContext::lookupValue(const Symbol* sym) {
	if (!sym || !currentFunc_) {
		return nullptr;
	}
	const FuncSymbol* targetFn = sym->definingFunc();
	const FuncSymbol* walker = currentFunc_;
	llvm::Value* framePtr = currentFramePtr_;
	const FrameInfo* frameInfo = currentFrameInfo();

	// Walk up the static link chain to find the frame that captures the variable
	while (walker && walker != targetFn && framePtr && frameInfo) {
		auto* frameTy = frameInfo->getFrameType();
		if (!frameTy) {
			return nullptr;
		}
		auto* parentSlot = builder().CreateStructGEP(frameTy, framePtr, 0, "staticlink.slot");
		llvm::Type* parentTy = frameTy->getElementType(0);
		framePtr = builder().CreateLoad(parentTy, parentSlot, "staticlink.up");
		walker = walker->definingFunc();
		frameInfo = walker ? getFrameInfo(walker) : nullptr;
	}

	if (!frameInfo || !framePtr) {
		return nullptr;
	}

	auto idxOpt = frameInfo->getCapturedVarIndex(sym);
	if (!idxOpt.has_value()) {
		return nullptr;
	}

	// For captured variables, get the slot from the frame
	auto* targetFrameTy = frameInfo->getFrameType();
	unsigned idx = *idxOpt;
	llvm::Value* slotPtr = builder().CreateStructGEP(targetFrameTy, framePtr, idx, sym->getName() + ".slot");

	// For by-ref parameters, load the pointer stored in the slot
	if (sym->getKind() == Symbol::SymKind::PARAM) {
        const auto* paramSym = static_cast<const ParamSymbol*>(sym);

        if (paramSym->getPass() == Symbol::ParamPass::BY_REF) {
            llvm::Type* slotTy = targetFrameTy->getElementType(idx);
            return builder().CreateLoad(slotTy, slotPtr, sym->getName() + ".byref");
        }
    }

	return slotPtr;
}

void CodegenContext::enterFunction(const FuncSymbol* fn, llvm::Value* framePtr) {
	if (currentFunc_ || currentFramePtr_) {
		frameStack_.emplace_back(currentFunc_, currentFramePtr_);
	}
	currentFunc_ = fn;
	currentFramePtr_ = framePtr;
}

void CodegenContext::leaveFunction() {
	if (!frameStack_.empty()) {
		auto prev = frameStack_.back();
		frameStack_.pop_back();
		currentFunc_ = prev.first;
		currentFramePtr_ = prev.second;
	} else {
		currentFunc_ = nullptr;
		currentFramePtr_ = nullptr;
	}
}

const FuncSymbol* CodegenContext::currentFunc() const {
	return currentFunc_;
}

CodegenContext::FrameInfo* CodegenContext::currentFrameInfo() {
	return currentFunc_ ? getFrameInfo(currentFunc_) : nullptr;
}

const CodegenContext::FrameInfo* CodegenContext::currentFrameInfo() const {
	return currentFunc_ ? getFrameInfo(currentFunc_) : nullptr;
}

llvm::Value* CodegenContext::currentFramePtr() const {
	return currentFramePtr_;
}
