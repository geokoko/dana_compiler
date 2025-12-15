#pragma once

#include <cstddef>
#include <memory>
#include <optional>
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

class CodegenContext { 
private:
    std::unique_ptr<llvm::LLVMContext> ownedCtx_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;

	/* 
     * FrameInfo:
     * Represents the "Permanent Layout" of a function's stack frame.
     * This persist across multiple calls to the same function.
     */
    struct FrameInfo {
        /* Frame layout information fields */
		// Struct type representing the frame layout
		// if function has no child functions, this is nullptr
		// otherwise, this is an actual StructType
        llvm::StructType* frameTy = nullptr;
		// Map of captured variables to their indices in the frame
        std::unordered_map<const Symbol*, unsigned> capturedIndices;
    };

    /* 
     * ActiveFuncState:
     * Tracks the execution state of the function currently being compiled.
     */
    struct ActiveFuncState {
        const FuncSymbol* funcSym;
        llvm::Value* framePtr;   // Pointer to the current function's frame (null for leaf functions)
        llvm::Value* staticLink; // Explicit static link to parent frame

        // Transient locals: variables living in registers or allocas (Path B)
        std::unordered_map<const Symbol*, llvm::Value*> localAddrs;

        // Loop control stack for the current function scope
        struct LoopInfo {
            llvm::BasicBlock* breakBB;
            llvm::BasicBlock* continueBB;
        };
        std::vector<LoopInfo> loopStack;
    };

    /* Stack of active functions for nested function generation */
    std::vector<ActiveFuncState> funcStack_;

    /* Frame Layout Definitions */
    std::unordered_map<const FuncSymbol*, std::unique_ptr<FrameInfo>> frameLayouts_;

    /* Generated Functions Cache */
    std::unordered_map<const FuncSymbol*, llvm::Function*> llvmFunctions_;

public:
    explicit CodegenContext(const std::string& moduleName);
    ~CodegenContext() = default;

    // LLVM Core Accessors
    llvm::LLVMContext& llvmContext() { return *ownedCtx_; }
    const llvm::LLVMContext& llvmContext() const { return *ownedCtx_; }
    llvm::Module& llvmModule()       { return *module_; }
    const llvm::Module& llvmModule() const { return *module_; }
    llvm::IRBuilder<>& builder()     { return *builder_; }
    const llvm::IRBuilder<>& builder() const { return *builder_; }

    // Type Translation
    llvm::Type* getLLVMType(const SemaType& ty, bool forParam = false);

    // Function Registry
    llvm::Function* lookupFunction(const FuncSymbol* sym) const;
    void bindFunction(const FuncSymbol* sym, llvm::Function* fn);

    // Frame Info Management
    FrameInfo* createFrameInfo(const FuncSymbol* fn);
    const FrameInfo* getFrameInfo(const FuncSymbol* fn) const;
    FrameInfo* getFrameInfo(const FuncSymbol* fn);

    // Scope / Function Entry & Exit
    void enterFunction(const FuncSymbol* fn, llvm::Value* framePtr, llvm::Value* staticLink);
    void leaveFunction();

    // Helper Methods
    void bindLocal(const Symbol* sym, llvm::Value* val);
    void bindStaticLink(llvm::Value* link);
    llvm::Value* lookupValue(const Symbol* sym);

    // Loop Control
    void pushLoop(llvm::BasicBlock* breakBB, llvm::BasicBlock* continueBB);
    void popLoop();
    llvm::BasicBlock* currentBreakTarget() const;
    llvm::BasicBlock* currentContinueTarget() const;

    // Current State Accessors
    const FuncSymbol* currentFunc() const;
    llvm::Value* currentFramePtr() const;
    llvm::Value* currentStaticLink() const;
};
