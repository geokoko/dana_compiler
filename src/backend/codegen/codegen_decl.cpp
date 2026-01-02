#include "codegen.hpp"

#include <vector>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

#include "../../frontend/ast/ast.hpp"

/* ========== Helper Structures & Functions ========== */
struct FuncSignature {
	bool needsStaticLink;
	llvm::Type* staticLinkTy;
	std::vector<llvm::Type*> paramTys;
	llvm::Type* retTy;
};

static FuncSignature buildSignature(CodegenContext& genCtx, const FuncSymbol* funcSym, bool is_main = false) {
	FuncSignature sig;
	sig.needsStaticLink = funcSym && funcSym->definingFunc() != nullptr;
	sig.staticLinkTy = llvm::PointerType::get(genCtx.llvmContext(), 0);
	sig.paramTys.clear();

	if (funcSym) {
		for (const auto& p : funcSym->getParams()) {
			const bool byRef = p->getPass() == Symbol::ParamPass::BY_REF;
			llvm::Type* ty = byRef ? llvm::PointerType::get(genCtx.llvmContext(), 0)
				: genCtx.getLLVMType(*p->getType(), /*forParam=*/true);
			sig.paramTys.push_back(ty);
		}
	}

	if (!is_main) {
		auto* fnSig = funcSym ? static_cast<const FuncType*>(funcSym->getType().get()) : nullptr;
		sig.retTy = (fnSig && fnSig->returnType()) ? genCtx.getLLVMType(*fnSig->returnType())
			: llvm::Type::getVoidTy(genCtx.llvmContext());
	}
	else {
		// override for "main"
		// force i32 return type
		sig.retTy = llvm::Type::getInt32Ty(genCtx.llvmContext());
		// clear parameters, just in case
		sig.paramTys.clear();
	}

	return sig;
}

static llvm::Function* ensureLLVMFunction(CodegenContext& genCtx, const FuncSymbol* funcSym, const FuncSignature& sig,
										  const bool is_main = false) {
	if (!funcSym) return nullptr;
	auto* llvmFunc = genCtx.lookupFunction(funcSym);
	if (llvmFunc) return llvmFunc;

	std::vector<llvm::Type*> fnArgTypes;
	if (sig.needsStaticLink) {
		fnArgTypes.push_back(sig.staticLinkTy);
	}
	fnArgTypes.insert(fnArgTypes.end(), sig.paramTys.begin(), sig.paramTys.end());

	auto* llvmFnTy = llvm::FunctionType::get(sig.retTy, fnArgTypes, /*isVarArg=*/false);
	llvmFunc = llvm::Function::Create(llvmFnTy, llvm::GlobalValue::ExternalLinkage,
								   is_main ? "main" : funcSym->getName(), &genCtx.llvmModule());
	genCtx.bindFunction(funcSym, llvmFunc);
	return llvmFunc;
}

static bool hasNestedFuncDefs(const vec<up<Def>>& defs) {
	for (const auto& defn : defs) {
		if (dynamic_cast<FuncDef*>(defn.get())) {
			return true;
		}
	}
	return false;
}

/* ========== Trivial generators for types/headers (no IR) ========== */

void Codegen::visit(Type& n)      { (void)n; }
void Codegen::visit(FParType& n)  { (void)n; }
void Codegen::visit(FParDef& n)   { (void)n; }
void Codegen::visit(VarDef& n)    { (void)n; }
void Codegen::visit(Header& n)    { (void)n; }

/* ========== Program ========== */

void Codegen::visit(Program& n) {
	if (auto* def = n.definition()) {
		def->accept(*this);
	}
}

/* ========== Functions ========== */
void Codegen::visit(FuncDecl& n) {
	/* Resolve Function Symbol & Header */
	auto* header = n.funcHeader();
	auto* funcSym = header ? header->symbol() : nullptr;
	if (!funcSym) {
		value = nullptr;
		return;
	}

	/* Analyze and declare */
	auto sig = buildSignature(genCtx, funcSym);
	ensureLLVMFunction(genCtx, funcSym, sig);

	value = nullptr;
}

void Codegen::visit(FuncDef& n) {
	/* Resolve Function Symbol & Header */
	auto* header = n.funcHeader();
	auto* funcSym = header ? header->symbol() : nullptr;
	if (!funcSym) {
		value = nullptr;
		return;
	}

	const bool is_main = n.isEntrypoint();

	/* Optimization Analysis: Do we need a physical frame? 
	 * Note: A physical frame for a function is needed if it has nested functions 
	 * (it is not a leaf function).
	 * */
	const bool needsFrame = hasNestedFuncDefs(n.localDefs());
	auto sig = buildSignature(genCtx, funcSym, is_main);
	auto* frameInfo = genCtx.getFrameInfo(funcSym);
	if (needsFrame && !frameInfo) {
		frameInfo = genCtx.createFrameInfo(funcSym);
	}

	/* Lookup or create the LLVM Function */
	llvm::Function* llvmFunc = ensureLLVMFunction(genCtx, funcSym, sig, is_main);
	if (!llvmFunc) {
		value = nullptr;
		return;
	}

	/* Begin current Function Generation by creating a BB */
	auto* entry = llvm::BasicBlock::Create(genCtx.llvmContext(), "entry", llvmFunc);
	genCtx.builder().SetInsertPoint(entry);

	auto argIt = llvmFunc->arg_begin(); 

	/* Needs static link if it has a parent function */
	llvm::Value* incomingStaticLink = nullptr;
	if (sig.needsStaticLink) {
		incomingStaticLink = &*argIt++;
		incomingStaticLink->setName("staticlink.in");
	}

	// PATH A: Nested Children Exist -> Create Physical Stack Frame (Struct)
	if (needsFrame) {
		std::vector<llvm::Type*> structFields;
		structFields.push_back(sig.staticLinkTy); // Field 0: Static Link

		size_t fieldIndex = 1;
		size_t paramIndex = 0;

		// Add Parameters
		for (const auto& p : funcSym->getParams()) {
			structFields.push_back(sig.paramTys[paramIndex++]);
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
			genCtx.builder().CreateStore(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(sig.staticLinkTy)), slPtr);
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

	/* Predeclare nested functions (defs and decls) */
	for (auto& def : n.localDefs()) {
		if (auto* decl = dynamic_cast<FuncDecl*>(def.get())) {
			decl->accept(*this);
			continue;
		}
		if (auto* defn = dynamic_cast<FuncDef*>(def.get())) {
			auto* nestedHeader = defn->funcHeader();
			auto* nestedSym = nestedHeader ? nestedHeader->symbol() : nullptr;
			auto nestedSig = buildSignature(genCtx, nestedSym);
			ensureLLVMFunction(genCtx, nestedSym, nestedSig);
		}
	}

	/* Generate Nested Function Local Defs */
	auto savedIP = genCtx.builder().saveIP();
	for (auto& def : n.localDefs()) {
		if (auto* nestedFunc = dynamic_cast<FuncDef*>(def.get())) {
			nestedFunc->accept(*this);
		}
	}
	genCtx.builder().restoreIP(savedIP);

	/* Generate Function Body */
	if (n.funcBody()) {
		n.funcBody()->accept(*this);
	}
	/* Ensure Terminator */
	llvm::BasicBlock* curBB = genCtx.builder().GetInsertBlock();
	if (curBB && !curBB->getTerminator()) {
		if (sig.retTy->isVoidTy()) {
			genCtx.builder().CreateRetVoid();
		} else {
			genCtx.builder().CreateRet(llvm::UndefValue::get(sig.retTy));
		}
	}

	/* Cleanup */
	genCtx.leaveFunction();
	value = nullptr;
}
