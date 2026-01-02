#include "codegen_context.hpp"


CodegenContext::CodegenContext(const std::string& moduleName) {
	ownedCtx_ = std::make_unique<llvm::LLVMContext>();
	module_ = std::make_unique<llvm::Module>(moduleName, *ownedCtx_);
	builder_ = std::make_unique<llvm::IRBuilder<>>(*ownedCtx_);
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
				return llvm::PointerType::get(elemTy, 0); 
			}

			auto* llvmArrTy = llvm::ArrayType::get(elemTy, *arrTy.size());
			return static_cast<llvm::Type*>(llvmArrTy);
		}
		case SemaType::TypeKind::FUNC: {
			// forParam = false; // function types never decay in this logic
			const auto& fnTy = static_cast<const FuncType&>(ty);
			std::vector<llvm::Type*> paramTypes;
			paramTypes.reserve(fnTy.params().size());

			for (const auto& p : fnTy.params()) {
				if (p) {
					llvm::Type* pt = getLLVMType(*p, /*forParam=*/true);
					paramTypes.push_back(pt);
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

llvm::Function* CodegenContext::lookupFunction(const FuncSymbol* sym) const {
	auto it = llvmFunctions_.find(sym);
	return it == llvmFunctions_.end() ? nullptr : it->second;
}

void CodegenContext::bindFunction(const FuncSymbol* sym, llvm::Function* fn) {
	if (sym) {
		llvmFunctions_[sym] = fn;
	}
}

CodegenContext::FrameInfo* CodegenContext::createFrameInfo(const FuncSymbol* fn) {
	if (!fn) return nullptr;
	auto info = std::make_unique<FrameInfo>();
	FrameInfo* ptr = info.get();
	frameLayouts_[fn] = std::move(info);
	return ptr;
}

const CodegenContext::FrameInfo* CodegenContext::getFrameInfo(const FuncSymbol* fn) const {
	auto it = frameLayouts_.find(fn);
	return it == frameLayouts_.end() ? nullptr : it->second.get();
}

CodegenContext::FrameInfo* CodegenContext::getFrameInfo(const FuncSymbol* fn) {
	auto it = frameLayouts_.find(fn);
	return it == frameLayouts_.end() ? nullptr : it->second.get();
}

void CodegenContext::enterFunction(const FuncSymbol* fn, llvm::Value* framePtr, llvm::Value* staticLink) {
	ActiveFuncState state;
	state.funcSym = fn;
	state.framePtr = framePtr;
	state.staticLink = staticLink;
	funcStack_.push_back(std::move(state));
}

void CodegenContext::leaveFunction() {
	if (!funcStack_.empty()) {
		funcStack_.pop_back();
	}
}

void CodegenContext::bindLocal(const Symbol* sym, llvm::Value* val) {
	if (!funcStack_.empty() && sym) {
		funcStack_.back().localAddrs[sym] = val;
	}
}

void CodegenContext::bindStaticLink(llvm::Value* link) {
	if (!funcStack_.empty()) {
		funcStack_.back().staticLink = link;
	}
}

llvm::Value* CodegenContext::lookupValue(const Symbol* sym) {
	if (!sym || funcStack_.empty()) {
		return nullptr;
	}

	// 1. Check transient locals in current function (registers/allocas)
	const auto& currentState = funcStack_.back();
	auto localIt = currentState.localAddrs.find(sym);
	if (localIt != currentState.localAddrs.end()) {
		llvm::Value* val = localIt->second;
		if (sym->getKind() == Symbol::SymKind::PARAM) {
			const auto* paramSym = static_cast<const ParamSymbol*>(sym);
			if (paramSym->getPass() == Symbol::ParamPass::BY_REF) {
				// val is an alloca of a pointer (ptr*). We need to load the pointer (ptr).
				return builder_->CreateLoad(builder_->getPtrTy(0), val, sym->getName() + ".ref");
			}
		}
		return val;
	}

	// 2. Frame Walk (for captured variables or variables in parent scopes)
	const FuncSymbol* target = sym->definingFunc();
	const FuncSymbol* walker = currentState.funcSym;
	llvm::Value* ptr = currentState.framePtr;
	if (!ptr) {
		ptr = currentState.staticLink;
		if (walker) {
			walker = walker->definingFunc();
		}
	}

	if (!ptr) {
		return nullptr;
	}

	while (walker) {
		const FrameInfo* info = getFrameInfo(walker);
		if (!info) break;

		// Check if the symbol is captured in THIS frame
		auto it = info->capturedIndices.find(sym);
		if (it != info->capturedIndices.end()) {
			// found
			unsigned idx = it->second;
			llvm::Value* slotPtr = builder_->CreateStructGEP(info->frameTy, ptr, idx, sym->getName() + ".slot");

			// If it's a By-Ref parameter, the slot contains the pointer, so we must load it
			if (sym->getKind() == Symbol::SymKind::PARAM) {
				const auto* paramSym = static_cast<const ParamSymbol*>(sym);
				if (paramSym->getPass() == Symbol::ParamPass::BY_REF) {
					return builder_->CreateLoad(info->frameTy->getElementType(idx), slotPtr, sym->getName() + ".ref");
				}
			}
			return slotPtr;
		}

		if (walker == target) break;

		if (!info->frameTy) break;
		
		// Move up to the parent frame
		auto* linkSlot = builder_->CreateStructGEP(info->frameTy, ptr, 0, "up.slot");
		// Load the pointer to the parent frame
		ptr = builder_->CreateLoad(info->frameTy->getElementType(0), linkSlot, "up.link");
		
		walker = walker->definingFunc();
	}

	return nullptr;
}

void CodegenContext::pushLoop(llvm::BasicBlock* breakBB, llvm::BasicBlock* continueBB) {
	if (!funcStack_.empty()) {
		funcStack_.back().loopStack.push_back({breakBB, continueBB});
	}
}

void CodegenContext::popLoop() {
	if (!funcStack_.empty() && !funcStack_.back().loopStack.empty()) {
		funcStack_.back().loopStack.pop_back();
	}
}

llvm::BasicBlock* CodegenContext::currentBreakTarget() const {
	if (funcStack_.empty() || funcStack_.back().loopStack.empty()) {
		return nullptr;
	}
	return funcStack_.back().loopStack.back().breakBB;
}

llvm::BasicBlock* CodegenContext::currentContinueTarget() const {
	if (funcStack_.empty() || funcStack_.back().loopStack.empty()) {
		return nullptr;
	}
	return funcStack_.back().loopStack.back().continueBB;
}

const FuncSymbol* CodegenContext::currentFunc() const {
	return funcStack_.empty() ? nullptr : funcStack_.back().funcSym;
}

llvm::Value* CodegenContext::currentFramePtr() const {
	return funcStack_.empty() ? nullptr : funcStack_.back().framePtr;
}

llvm::Value* CodegenContext::currentStaticLink() const {
	return funcStack_.empty() ? nullptr : funcStack_.back().staticLink;
}
