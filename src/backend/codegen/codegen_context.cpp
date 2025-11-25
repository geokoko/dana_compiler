#include "codegen_context.hpp"

CodegenContext::CodegenContext(const std::string& moduleName) {
	ownedCtx_ = std::make_unique<llvm::LLVMContext>();
	module_ = std::make_unique<llvm::Module>(moduleName, *ownedCtx_);
	builder_ = std::make_unique<llvm::IRBuilder<>>(*ownedCtx_);
}


