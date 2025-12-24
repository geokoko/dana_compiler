#include "codegen.hpp"

#include <vector>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

#include "../../frontend/ast/ast.hpp"

/* ========== Trivial generators for types/headers (no IR) ========== */

void LLVMCodegen::gen(Type& n)      { (void)n; }
void LLVMCodegen::gen(FParType& n)  { (void)n; }
void LLVMCodegen::gen(FParDef& n)   { (void)n; }
void LLVMCodegen::gen(VarDef& n)    { (void)n; }
void LLVMCodegen::gen(Header& n)    { (void)n; }
void LLVMCodegen::gen(FuncDecl& n)  { (void)n; }

/* ========== Program ========== */

void LLVMCodegen::gen(Program& n) {
    if (auto* def = n.definition()) {
        def->accept(*this);
    }
}

/* ========== Functions ========== */
void LLVMCodegen::gen(FuncDef& n) {
    /* 1. Resolve Function Symbol & Header */
    auto* header = n.funcHeader();
    auto* funcSym = header ? header->symbol() : nullptr;
    if (!funcSym) {
        value = nullptr;
        return;
    }

    /* 2. Optimization Analysis: Do we need a physical frame? */
    bool needsFrame = false;
    for (auto& defn : n.localDefs()) {
        if (dynamic_cast<FuncDef*>(defn.get())) {
            needsFrame = true;
            break;
        }
    }

    auto* frameInfo = genCtx.createFrameInfo(funcSym);

    /* 3. Prepare Type Definitions */
    std::vector<llvm::Type*> paramTys;
    for (const auto& p : funcSym->getParams()) {
        const bool byRef = p->getPass() == Symbol::ParamPass::BY_REF;
        llvm::Type* ty = byRef ? llvm::PointerType::get(genCtx.llvmContext(), 0) 
            : genCtx.getLLVMType(*p->getType(), /*forParam=*/true);
        paramTys.push_back(ty);
    }

    bool needsStaticLink = funcSym->definingFunc() != nullptr;
    llvm::Type* staticLinkTy = llvm::PointerType::get(genCtx.llvmContext(), 0);

    /* 4. Build LLVM Function Signature */
    std::vector<llvm::Type*> fnArgTypes;
    if (needsStaticLink) {
        fnArgTypes.push_back(staticLinkTy);
    }
    fnArgTypes.insert(fnArgTypes.end(), paramTys.begin(), paramTys.end());

    auto* sig = static_cast<const FuncType*>(funcSym->getType().get());
    llvm::Type* retTy = (sig && sig->returnType()) ? genCtx.getLLVMType(*sig->returnType()) 
        : llvm::Type::getVoidTy(genCtx.llvmContext());

    auto* llvmFnTy = llvm::FunctionType::get(retTy, fnArgTypes, /*isVarArg=*/false);

    /* 5. Create or Lookup LLVM Function */
    llvm::Function* llvmFunc = genCtx.lookupFunction(funcSym);
    if (!llvmFunc) {
        llvmFunc = llvm::Function::Create(llvmFnTy, llvm::GlobalValue::ExternalLinkage, 
                                    funcSym->getName(), &genCtx.llvmModule());
        genCtx.bindFunction(funcSym, llvmFunc);
    }

    /* 6. Generate Nested Functions First (Recursion) */
    for (auto& def : n.localDefs()) {
        if (auto* nestedFunc = dynamic_cast<FuncDef*>(def.get())) {
            nestedFunc->accept(*this);
        }
    }

    /* 7. Begin Function Generation */
    auto* entry = llvm::BasicBlock::Create(genCtx.llvmContext(), "entry", llvmFunc);
    genCtx.builder().SetInsertPoint(entry);

    auto argIt = llvmFunc->arg_begin();
    llvm::Value* incomingStaticLink = nullptr;

    if (needsStaticLink) {
        incomingStaticLink = &*argIt++;
        incomingStaticLink->setName("staticlink.in");
    }

    // PATH A: Nested Children Exist -> Create Physical Stack Frame (Struct)
    if (needsFrame) {
        std::vector<llvm::Type*> structFields;
        structFields.push_back(staticLinkTy); // Field 0: Static Link

        size_t fieldIndex = 1;

        // Add Parameters
        for (const auto& p : funcSym->getParams()) {
            structFields.push_back(paramTys[fieldIndex - 1]);
            frameInfo->capturedIndices[p] = fieldIndex++;
        }

        // Add Locals
        for (auto& def : n.localDefs()) {
            if (auto* var = dynamic_cast<VarDef*>(def.get())) {
                for (auto* sym : var->symbols()) {
                    structFields.push_back(genCtx.getLLVMType(*sym->getType()));
                    frameInfo->capturedIndices[sym] = fieldIndex++;
                }
            }
        }

        llvm::StructType* frameStructTy = llvm::StructType::create(genCtx.llvmContext(), structFields, funcSym->getName() + ".frame");
        frameInfo->frameTy = frameStructTy;

        auto* framePtr = genCtx.builder().CreateAlloca(frameStructTy, nullptr, funcSym->getName() + ".frame");
        genCtx.enterFunction(funcSym, framePtr, incomingStaticLink);

        // Store Static Link
        llvm::Value* slPtr = genCtx.builder().CreateStructGEP(frameStructTy, framePtr, 0, "staticlink.ptr");
        if (incomingStaticLink) {
            genCtx.builder().CreateStore(incomingStaticLink, slPtr);
        } else {
            genCtx.builder().CreateStore(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(staticLinkTy)), slPtr);
        }

        // Store Parameters
        for (const auto& p : funcSym->getParams()) {
            llvm::Value* argVal = &*argIt++;
            argVal->setName(p->getName());
            
            auto idx = frameInfo->capturedIndices[p];
            // assert(idxOpt.has_value());
            llvm::Value* paramGEP = genCtx.builder().CreateStructGEP(frameStructTy, framePtr, idx, p->getName() + ".ptr");
            genCtx.builder().CreateStore(argVal, paramGEP);
            genCtx.bindLocal(p, paramGEP);
        }

        // Bind Locals
        for (auto& def : n.localDefs()) {
            if (auto* var = dynamic_cast<VarDef*>(def.get())) {
                for (auto* sym : var->symbols()) {
                    auto idx = frameInfo->capturedIndices[sym];
                    llvm::Value* localGEP = genCtx.builder().CreateStructGEP(frameStructTy, framePtr, idx, sym->getName() + ".ptr");
                    genCtx.bindLocal(sym, localGEP);
                }
            }
        }
    }
    // PATH B: Leaf Function -> Flat Optimization
    else {
        // No physical frame, but we still track static link for logic
        genCtx.enterFunction(funcSym, nullptr, incomingStaticLink);

        // Individual Allocas for Parameters
        for (const auto& p : funcSym->getParams()) {
            llvm::Value* argVal = &*argIt++;
            argVal->setName(p->getName());

            llvm::AllocaInst* allocaInst = genCtx.builder().CreateAlloca(argVal->getType(), nullptr, p->getName() + ".addr");
            genCtx.builder().CreateStore(argVal, allocaInst);
            genCtx.bindLocal(p, allocaInst);
        }

        // Individual Allocas for Locals
        for (auto& def : n.localDefs()) {
            if (auto* var = dynamic_cast<VarDef*>(def.get())) {
                for (auto* sym : var->symbols()) {
                    llvm::Type* ty = genCtx.getLLVMType(*sym->getType());
                    llvm::AllocaInst* allocaInst = genCtx.builder().CreateAlloca(ty, nullptr, sym->getName());
                    genCtx.bindLocal(sym, allocaInst);
                }
            }
        }
    }

    /* 8. Generate Function Body */
    if (n.funcBody()) {
        n.funcBody()->accept(*this);
    }

    /* 9. Ensure Terminator */
    llvm::BasicBlock* curBB = genCtx.builder().GetInsertBlock();
    if (curBB && !curBB->getTerminator()) {
        if (retTy->isVoidTy()) {
            genCtx.builder().CreateRetVoid();
        } else {
            genCtx.builder().CreateRet(llvm::UndefValue::get(retTy));
        }
    }

    /* 10. Cleanup */
    genCtx.leaveFunction();
    value = nullptr;
}
