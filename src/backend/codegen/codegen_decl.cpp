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
		def->agen(*this);
	}
}

/* ========== Functions ========== */
void LLVMCodegen::gen(FuncDef& n) {
	/* Resolve function symbol */
	auto* header  = n.funcHeader();
	auto* funcSym = header ? header->symbol() : nullptr;
	if (!funcSym) {
		value = nullptr;
		return;
	}

	// Per-function frame info
	auto* frameInfo = genCtx.createFrameInfo(funcSym);

	/* Determine parent frame type for static link (not needed for layout, kept for clarity) */
	llvm::StructType* parentFrameTy = nullptr;
	if (auto* parentSym = funcSym->definingFunc()) {
		if (const auto* parentInfo = genCtx.getFrameInfo(parentSym)) {
			parentFrameTy = parentInfo->getFrameType();
			(void)parentFrameTy;
		}
	}

	/* Build frame layout */
	std::vector<llvm::Type*> paramTys;
	std::vector<llvm::Type*> localTys;

	// Field 0: static link
	llvm::Type* staticLinkTy = llvm::PointerType::get(genCtx.llvmContext(), 0);

	std::size_t fieldIndex = 1; // 0 reserved for static link

	// Parameters
	for (const auto& p : funcSym->getParams()) {
		const bool byRef = p->getPass() == Symbol::ParamPass::BY_REF;
		llvm::Type* slotTy = nullptr;

		if (byRef) {
			slotTy = llvm::PointerType::get(genCtx.llvmContext(), 0);
		} else {
			slotTy = genCtx.getLLVMType(*p->getType(), /*forParam=*/true);
		}

		paramTys.push_back(slotTy);
		frameInfo->captureVar(p.get(), fieldIndex++);
	}

	// Local variables
	for (auto& def : n.localDefs()) {
		if (auto* var = dynamic_cast<VarDef*>(def.get())) {
			for (auto* sym : var->symbols()) {
				localTys.push_back(genCtx.getLLVMType(*sym->getType()));
				frameInfo->captureVar(sym, fieldIndex++);
			}
		}
	}

	// Create or update frame struct type
	if (!frameInfo->getFrameType()) {
		frameInfo->setFrameType(llvm::StructType::create(genCtx.llvmContext(), funcSym->getName() + ".frame"));
	}
	auto* frameTy = frameInfo->getFrameType();

	std::vector<llvm::Type*> fieldTys;
	fieldTys.reserve(1 + paramTys.size() + localTys.size());
	fieldTys.push_back(staticLinkTy);
	fieldTys.insert(fieldTys.end(), paramTys.begin(), paramTys.end());
	fieldTys.insert(fieldTys.end(), localTys.begin(), localTys.end());

	if (frameTy->isOpaque()) {
		frameTy->setBody(fieldTys, /*isPacked=*/false);
	}

	// Build function type
	auto* sig = static_cast<const FuncType*>(funcSym->getType().get());
	llvm::Type* retTy = sig && sig->returnType() ? genCtx.getLLVMType(*sig->returnType()) : llvm::Type::getVoidTy(genCtx.llvmContext());

	std::vector<llvm::Type*> fnParamTypes;
	bool needsStaticLink = funcSym->definingFunc() != nullptr;
	if (needsStaticLink) {
		fnParamTypes.push_back(staticLinkTy);
	}
	fnParamTypes.insert(fnParamTypes.end(), paramTys.begin(), paramTys.end());

	auto* llvmFnTy = llvm::FunctionType::get(retTy, fnParamTypes, /*isVarArg=*/false);

	// Create or reuse LLVM function
	llvm::Function* llvmFunc = genCtx.lookupFunction(funcSym);
	if (!llvmFunc) {
		llvmFunc = llvm::Function::Create(llvmFnTy, llvm::GlobalValue::ExternalLinkage, funcSym->getName(), &genCtx.llvmModule());
		genCtx.bindFunction(funcSym, llvmFunc);
	}

	// Entry block + frame allocation
	auto* entry = llvm::BasicBlock::Create(genCtx.llvmContext(), "entry", llvmFunc);
	genCtx.builder().SetInsertPoint(entry);

	auto* framePtr = genCtx.builder().CreateAlloca(frameTy, nullptr, funcSym->getName() + ".frame");

	// Enter function context
	genCtx.enterFunction(funcSym, framePtr);

	/* Bind static link and parameters to frame fields */
	auto argIt = llvmFunc->arg_begin();

	if (needsStaticLink) {
		argIt->setName("staticlink.arg");
		llvm::Value* staticLinkArg = &*argIt++;
		llvm::Value* staticLinkPtr = genCtx.builder().CreateStructGEP(frameTy, framePtr, 0, "staticlink.ptr");
		genCtx.builder().CreateStore(staticLinkArg, staticLinkPtr);
	} else {
		llvm::Value* staticLinkPtr = genCtx.builder().CreateStructGEP(frameTy, framePtr, 0, "staticlink.ptr");
		auto* ptrTy = llvm::cast<llvm::PointerType>(staticLinkTy);
		llvm::Value* nullPtr = llvm::ConstantPointerNull::get(ptrTy);
		genCtx.builder().CreateStore(nullPtr, staticLinkPtr);
	}

	// Bind parameters into the frame 
	for (const auto& p : funcSym->getParams()) {
		auto idxOpt = frameInfo->getCapturedVarIndex(p.get());
		assert(idxOpt.has_value());
		std::size_t idx = *idxOpt;

		llvm::Value* paramArg = &*argIt++;
		llvm::Value* paramPtr = genCtx.builder().CreateStructGEP(frameTy, framePtr, idx, p->getName() + ".ptr");
		genCtx.builder().CreateStore(paramArg, paramPtr);
	}

	// Locals: only slots in the frame 
	for (auto& def : n.localDefs()) {
		if (auto* var = dynamic_cast<VarDef*>(def.get())) {
			for (auto* sym : var->symbols()) {
				auto idxOpt = frameInfo->getCapturedVarIndex(sym);
				assert(idxOpt.has_value());
				std::size_t idx = *idxOpt;
				llvm::Value* localPtr = genCtx.builder().CreateStructGEP(frameTy, framePtr, idx, sym->getName() + ".ptr");
			}
		}
	}

	/* Generate function body */
	if (n.funcBody()) {
		n.funcBody()->agen(*this);
	}

	/* Default return if none emitted */
	if (!entry->getTerminator()) {
		if (retTy->isVoidTy()) {
			genCtx.builder().CreateRetVoid();
		} else {
			genCtx.builder().CreateRet(llvm::UndefValue::get(retTy));
		}
	}

	genCtx.leaveFunction();
	value = nullptr;
}

