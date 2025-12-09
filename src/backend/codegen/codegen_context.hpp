#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <utility>
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
public:
	/* Frame logic */
	class FrameInfo {
	public:
		FrameInfo() = default;
		~FrameInfo() = default;
		/* Frame layout information */
		void setFrameType(llvm::StructType* frameType);
		llvm::StructType* getFrameType();
		llvm::StructType* getFrameType() const;

		/* Symbol to LLVM value mapping */
		void captureVar(const Symbol* sym, std::size_t index);
		std::optional<std::size_t> getCapturedVarIndex(const Symbol* sym) const;

		/* Loop control */
		void pushLoop(llvm::BasicBlock* breakBB, llvm::BasicBlock* continueBB);
		void popLoop();
		llvm::BasicBlock* currentBreakTarget() const;
		llvm::BasicBlock* currentContinueTarget() const;

	private:
		llvm::StructType* frameTy = nullptr; // LLVM struct type representing the frame layout
		std::unordered_map<const Symbol*, llvm::Value*> valueMap_; // symbol to LLVM value mapping for locals in this frame
		std::unordered_map<const Symbol*, std::size_t> capturedVars; // locals mapping to their indices in the frame
		
		/* Storage of loop targets */
		std::vector<llvm::BasicBlock*> breakTargets;
		std::vector<llvm::BasicBlock*> continueTargets;
	};

private:
	std::unique_ptr<llvm::LLVMContext> ownedCtx_;
	std::unique_ptr<llvm::Module> module_;
	std::unique_ptr<llvm::IRBuilder<>> builder_;
	
	/* Mapping of Dana function symbols to LLVM entities */
	std::unordered_map<const FuncSymbol*, llvm::Function*> functionMap_;
	std::unordered_map<const FuncSymbol*, std::unique_ptr<FrameInfo>> frameMap_;
	/* Stack of active functions (with their frame pointer) for nested function generation */
	std::vector<std::pair<const FuncSymbol*, llvm::Value*>> frameStack_;

	/* Current function being generated */
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

	/* Semantic Type to LLVM type translation
	 * Type translation to an LLVM type. 
	 * If forParam is true, unsized arrays/by-ref params decay to pointers.
	 * */
	llvm::Type* getLLVMType(const SemaType& ty, bool forParam = false);

	/* FrameInfo management */
	FrameInfo* createFrameInfo(const FuncSymbol* fn);
	const FrameInfo* getFrameInfo(const FuncSymbol* fn) const;
	FrameInfo* getFrameInfo(const FuncSymbol* fn);

	/* Dana Symbol to LLVM value translation */
	llvm::Value* lookupValue(const Symbol* sym);
	llvm::Function* lookupFunction(const FuncSymbol* sym) const;
	llvm::Function* getLLVMFunction(const FuncSymbol* fn);
	void bindFunction(const FuncSymbol* sym, llvm::Function* fn);

	// Track the function currently being generated and its frame pointer.
	void enterFunction(const FuncSymbol* fn, llvm::Value* framePtr);
	void leaveFunction();

	const FuncSymbol* currentFunc() const;
	FrameInfo* currentFrameInfo();
	const FrameInfo* currentFrameInfo() const;
	llvm::Value* currentFramePtr() const;
};
