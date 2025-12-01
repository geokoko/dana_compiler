#include "codegen_context.hpp"

CodegenContext::CodegenContext(const std::string& moduleName) {
	ownedCtx_ = std::make_unique<llvm::LLVMContext>();
	module_ = std::make_unique<llvm::Module>(moduleName, *ownedCtx_);
	builder_ = std::make_unique<llvm::IRBuilder<>>(*ownedCtx_);
}

CodegenContext::FrameInfo::FrameInfo(const Symbol* sym, llvm::Function* fn, llvm::BasicBlock* entry, llvm::StructType* frameType)
								: funcSymbol(sym), llvmFunc(fn), entryBlock(entry), frameTy(frameType) {}

llvm::Value* CodegenContext::FrameInfo::lookupValue(const Symbol* sym) const {
	auto it = valueMap_.find(sym);
	return it == valueMap_.end() ? nullptr : it->second;
}

void CodegenContext::FrameInfo::bindValue(const Symbol* sym, llvm::Value* value) {
	if (!sym) return;
	valueMap_[sym] = value;
}

void CodegenContext::FrameInfo::captureVar(const Symbol* sym, std::size_t index) {
	if (!sym) return;
	capturedVars[sym] = index;
}

llvm::Value* CodegenContext::lookupValue(const Symbol* sym) const {
	if (!sym || !currentFunc_) {
		return nullptr;
	}
	const auto* fi = getFrameInfo(currentFunc_);
	return fi ? fi->FrameInfo::lookupValue(sym) : nullptr;
}

void CodegenContext::bindValue(const Symbol* sym, llvm::Value* value) {
	if (!sym || !currentFunc_) {
		return;
	}
	auto* fi = getFrameInfo(currentFunc_);
	if (fi) {
		fi->bindValue(sym, value);
	}
}

llvm::Function* CodegenContext::lookupFunction(const FuncSymbol* sym) const {
	auto it = functionMap_.find(sym);
	return it == functionMap_.end() ? nullptr : it->second;
}

void CodegenContext::bindFunction(const FuncSymbol* sym, llvm::Function* fn) {
	if (!sym) return;
	functionMap_[sym] = fn;
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

			// Unsized arrays decay to pointers (primarily for parameters)
			if (!arrTy.size()) {
				return llvm::PointerType::get(elemTy, 0);
			}

			auto* llvmArrTy = llvm::ArrayType::get(elemTy, *arrTy.size());
			return forParam ? llvm::PointerType::get(llvmArrTy, 0) : llvmArrTy;
		}
		case SemaType::TypeKind::FUNC: {
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
			return forParam ? llvm::PointerType::get(llvmFnTy, 0) : llvmFnTy;
		}
	}
	return nullptr;
}

CodegenContext::FrameInfo* CodegenContext::getFrameInfo(const FuncSymbol* fn) {
	if (!fn) {
		return nullptr;
	}
	return &frameMap_[fn];
}

const CodegenContext::FrameInfo* CodegenContext::getFrameInfo(const FuncSymbol* fn) const {
	auto it = frameMap_.find(fn);
	return it == frameMap_.end() ? nullptr : &it->second;
}

void CodegenContext::enterFunction(const FuncSymbol* fn, llvm::Value* framePtr) {
	currentFunc_ = fn;
	currentFramePtr_ = framePtr;
}

void CodegenContext::leaveFunction() {
	currentFunc_ = nullptr;
	currentFramePtr_ = nullptr;
}

const FuncSymbol* CodegenContext::currentFunc() const {
	return currentFunc_;
}

llvm::Value* CodegenContext::currentFramePtr() const {
	return currentFramePtr_;
}


