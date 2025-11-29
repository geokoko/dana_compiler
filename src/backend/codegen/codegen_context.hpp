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
#include <llvm/IR/Instructions.h>

#include "../../frontend/symbol/symbol.hpp"
#include "../../frontend/symbol/sematype.hpp"

/* CodegenContext holds all LLVM state needed while generating IR for a Dana Program */

class CodegenContext {
private:
    std::unique_ptr<llvm::LLVMContext> ownedCtx_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;

	/* Frame logic */
	class FrameInfo {
	public:
		FrameInfo(const Symbol* sym = nullptr, llvm::Function* fn = nullptr,
			llvm::BasicBlock* entry = nullptr, llvm::StructType* frameType = nullptr);

		llvm::Value* lookupValue(const Symbol* sym) const;
		void bindValue(const Symbol* sym, llvm::Value* value);
		void captureVar(const Symbol* sym, std::size_t index);
		
	private:
		friend class LLVMCodegen; // allow codegen to populate frame layout
		const Symbol* funcSymbol = nullptr; // function this frame corresponds to
		llvm::Function* llvmFunc = nullptr; // LLVM function this frame corresponds to
		llvm::BasicBlock* entryBlock = nullptr; // entry block of the function
		llvm::StructType* frameTy = nullptr; // LLVM struct type representing the frame layout
    	std::unordered_map<const Symbol*, llvm::Value*> valueMap_;
    	std::unordered_map<const Symbol*, std::size_t> capturedVars; 
    	std::vector<llvm::BasicBlock*> breakTargets;
    	std::vector<llvm::BasicBlock*> continueTargets;
    	llvm::AllocaInst* returnSlot = nullptr;
	};

    /* Mapping of Dana symbols to LLVM entities */
	std::unordered_map<const FuncSymbol*, llvm::Function*> functionMap_;
	// Frame information per function
	std::unordered_map<const FuncSymbol*, FrameInfo> frameMap_;
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
    llvm::Function* lookupFunction(const FuncSymbol* sym) const;
    void bindFunction(const FuncSymbol* sym, llvm::Function* fn);

	/* Semantic Type to LLVM type translation
	 * Type translation to an LLVM type. 
	 * If forParam is true, unsized arrays/by-ref params decay to pointers.
	 * */
    llvm::Type* getLLVMType(const SemaType& ty, bool forParam = false);
	
	// Track the function currently being generated and its frame pointer.
	void enterFunction(const FuncSymbol* fn, llvm::Value* framePtr);
	void leaveFunction();
	const FuncSymbol* currentFunc() const;
	llvm::Value* currentFramePtr() const;
	// Frame information access
	FrameInfo* getFrameInfo(const FuncSymbol* fn);
	const FrameInfo* getFrameInfo(const FuncSymbol* fn) const;
};
