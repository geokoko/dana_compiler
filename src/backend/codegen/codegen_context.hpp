#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>

#include "../../frontend/symbol/symbol.hpp"
#include "../../frontend/symbol/sematype.hpp"

/* CodegenContext holds all LLVM state needed while generating IR for a Dana Program */

class CodegenContext {
private:
    std::unique_ptr<llvm::LLVMContext> ownedCtx_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;

	/* Frame logic */
	struct FrameInfo {
		bool hasFrame = false;
		// fieldIndex maps captured symbols to their field slot inside the frame struct.
		std::unordered_map<const Symbol*, std::size_t> fieldIndex;
	};

    // Mapping of Dana symbols to LLVM entities
	// Storing semantic symbol and LLVM Value. Fast lookup when in need
	// to call a symbol with an already resolved LLVM Value
    std::unordered_map<const Symbol*, llvm::Value*>    valueMap_;
	// Same as above, but for functions
    std::unordered_map<const Symbol*, llvm::Function*> functionMap_;
	// Mapping of Function Symbols to FrameInfo
	std::unordered_map<const FuncSymbol*, FrameInfo> frames_;
	const FuncSymbol* currentFunc_ = nullptr;
	llvm::Value* currentFramePtr_ = nullptr;

public:
    // Constructor of LLVMContext
    explicit CodegenContext(const std::string& moduleName);
   
	// Getters of LLVM core objects
    llvm::LLVMContext& llvmContext() { return *ownedCtx_; }
    const llvm::LLVMContext& llvmContext() const { return *ownedCtx_; }
    llvm::Module& llvmModule()       { return *module_; }
    const llvm::Module& llvmModule() const { return *module_; }
    llvm::IRBuilder<>& builder()     { return *builder_; }
    const llvm::IRBuilder<>& builder() const { return *builder_; }
    
	/* Dana Symbol to LLVM value translation */
    llvm::Value* lookupValue(const Symbol* sym) const;
    void bindValue(const Symbol* sym, llvm::Value* value);
    llvm::Function* lookupFunction(const Symbol* sym) const;
    void bindFunction(const Symbol* sym, llvm::Function* fn);

	/* Semantic Type to LLVM type translation
	 * Type translation to an LLVM type. 
	 * If forParam is true, unsized arrays/by-ref params decay to pointers.
	 * */
    llvm::Type* getLLVMType(const SemaType& ty, bool forParam = false);

	FrameInfo* frameInfo(const FuncSymbol* fn);

	// Track the function currently being generated and its frame pointer.
	void enterFunction(const FuncSymbol* fn, llvm::Value* framePtr);
	void leaveFunction();
	const FuncSymbol* currentFunc() const;
	llvm::Value* currentFramePtr() const;
};
