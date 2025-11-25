#pragma once

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

/* Codegen context holds all LLVM state needed while generating IR for a Dana Program */

class CodegenContext {
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
	// Return the LLVM Value currently associated with a Dana Symbol
    llvm::Value* lookupValue(const Symbol* sym) const;
    // Bind a Symbol to a specific LLVM Value
    void bindValue(const Symbol* sym, llvm::Value* value);
    // Return the LLVM Function associated with a function Symbol
    llvm::Function* lookupFunction(const Symbol* sym) const;
    // Bind a Symbol to a specific LLVM Function.
    void bindFunction(const Symbol* sym, llvm::Function* fn);

	/* Loop logic handling */
    struct LoopTargets {
        llvm::BasicBlock* breakTarget  = nullptr;
        llvm::BasicBlock* continueTarget = nullptr;
    };
    // Push loop targets when you enter a loop body.
    void pushLoop(LoopTargets);
    // Pop loop targets when you leave a loop.
    void popLoop();
    // Current loop break/continue targets
    llvm::BasicBlock* currentBreakTarget() const;
    llvm::BasicBlock* currentContinueTarget() const;

	/* Semantic Type to LLVM type translation */
    // Type translation to an LLVM type. If forParam is true, unsized arrays/by-ref params decay to pointers.
    llvm::Type* getLLVMType(const SemaType& ty, bool forParam = false);

    // Type getters
    llvm::Type* intTy();	// i32
    llvm::Type* byteTy();	// i8
    llvm::Type* voidTy();	// void
    llvm::Type* boolTy();	// i1
    // Conditions in Dana are byte-based; this returns i1 for branching and expects callers to cast as needed.
    llvm::Type* condTy() { return boolTy(); }

    // Run LLVM's verifier on the module. Returns true if valid, false otherwise.
    bool verifyModule() const;

private:
    std::unique_ptr<llvm::LLVMContext> ownedCtx_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;

    // Mapping of Dana symbols to LLVM entities
    std::unordered_map<const Symbol*, llvm::Value*>    valueMap_;
    std::unordered_map<const Symbol*, llvm::Function*> functionMap_;

    std::vector<LoopTargets> loopStack_;
};
