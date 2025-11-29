#include "codegen_context.hpp"

CodegenContext::CodegenContext(const std::string& moduleName) {
	ownedCtx_ = std::make_unique<llvm::LLVMContext>();
	module_ = std::make_unique<llvm::Module>(moduleName, *ownedCtx_);
	builder_ = std::make_unique<llvm::IRBuilder<>>(*ownedCtx_);
}

llvm::Value* CodegenContext::lookupValue(const Symbol* sym) const {
	auto it = valueMap_.find(sym);
	return it == valueMap_.end() ? nullptr : it->second;
}

void CodegenContext::bindValue(const Symbol* sym, llvm::Value* value) {
	if (!sym) return;
	valueMap_[sym] = value;
}

llvm::Function* CodegenContext::lookupFunction(const Symbol* sym) const {
	auto it = functionMap_.find(sym);
	return it == functionMap_.end() ? nullptr : it->second;
}

void CodegenContext::bindFunction(const Symbol* sym, llvm::Function* fn) {
	if (!sym) return;
	functionMap_[sym] = fn;
}

CodegenContext::FrameInfo* CodegenContext::frameInfo(const FuncSymbol* fn) {
	if (!fn) {
		return nullptr;
	}
	return &frames_[fn];
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
