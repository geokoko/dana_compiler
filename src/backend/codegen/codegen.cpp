#include "codegen.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
void LLVMCodegen::gen(Program& n) {
	if (auto* def = n.definition()) {
		def->agen(*this);
	}
}

void LLVMCodegen::gen(FuncDef& n) {
	auto* header = n.funcHeader();
	// fetch function symbol from header node
	auto* funcSym = header ? header->symbol() : nullptr;
	if (!funcSym) {
		return;
	}

	auto* sig = static_cast<const FuncType*>(funcSym->getType().get());

	// Store function parameter types as LLVM Types
	// and use them to create a LLVM function type
	std::vector<llvm::Type*> paramTypes;
	paramTypes.reserve(funcSym->getParams().size());
	for (const auto& p : funcSym->getParams()) {
		paramTypes.push_back(genCtx.getLLVMType(*p->getType(), /*forParam=*/true));
	}
	// capture function return type and store it as LLVM type
	auto* retTy = genCtx.getLLVMType(*sig->returnType());
	auto* fnTy = llvm::FunctionType::get(retTy, paramTypes, /*isVarArg=*/false);

	// map symbol to LLVM function (if not already done)
	auto* fn = genCtx.lookupFunction(funcSym);
	if (!fn) {
		fn = llvm::Function::Create(fnTy, llvm::Function::ExternalLinkage, funcSym->getName(), &genCtx.llvmModule());
		genCtx.bindFunction(funcSym, fn);
	}

	// Function definitions open scopes -> Basic Block in LLVM
	auto* entry = llvm::BasicBlock::Create(genCtx.llvmContext(), funcSym->getName() + "_entry", fn);
	genCtx.builder().SetInsertPoint(entry);
	// for mem allocation in stack frame
	llvm::IRBuilder<> allocaBuilder(&fn->getEntryBlock(), fn->getEntryBlock().begin());

	// bind param symbols, allocate memory and add them as function arguments to LLVM func
	std::size_t idx = 0;
	for (auto& arg : fn->args()) {
		arg.setName(funcSym->getParams()[idx]->getName());
		if (funcSym->getParams()[idx]->getPass() == Symbol::ParamPass::BY_REF) {
			// if passed by reference, do not allocate stack space; argument is a pointer
			genCtx.bindValue(funcSym->getParams()[idx].get(), &arg);
		} else {
			// allocate stack space
			auto* slot = allocaBuilder.CreateAlloca(arg.getType(), nullptr, arg.getName() + ".addr");
			genCtx.builder().CreateStore(&arg, slot);
			genCtx.bindValue(funcSym->getParams()[idx].get(), slot);
		}
		++idx;
	}

	// local defs handling
	for (auto& def : n.localDefs()) {
		if (auto* var = dynamic_cast<VarDef*>(def.get())) {
			const auto& syms = var->symbols();
			for (auto* sym : syms) {
				auto* ty = genCtx.getLLVMType(*sym->getType());
				// allocate stack space
				auto* slot = allocaBuilder.CreateAlloca(ty, nullptr, sym->getName());
				genCtx.bindValue(sym, slot);
			}
		}
	}

	// handle func body
	if (n.funcBody()) {
		n.funcBody()->agen(*this);
	}

	// if no terminator for current block, set return type
	if (!entry->getTerminator()) {
		if (retTy->isVoidTy()) genCtx.builder().CreateRetVoid();
		else genCtx.builder().CreateRet(llvm::UndefValue::get(retTy));
	}

	// no value to be returned
	value = nullptr;

}

void LLVMCodegen::gen(Block& n) {
	for (auto& stmt : n.statementsList()) {
		stmt->agen(*this);
	}
}

void LLVMCodegen::gen(SkipStmt& n) {
	(void)n;
}

void LLVMCodegen::gen(ExitStmt& n) {
	// Exit from a procedure: emit a void return
	genCtx.builder().CreateRetVoid();
}

void LLVMCodegen::gen(AssignStmt& n) {
	// generate code for the right-hand side expression
	if (auto* rhs = n.right()) {
		rhs->agen(*this);
	}

	// generate code for the left-hand side lvalue
	if (auto* lhs = n.left()) {
		lhs->agen(*this);
	}

	// perform the assignment
	llvm::Value* rhsValue = /* get the value from rhs */;
	llvm::Value* lhsAddress = /* get the address from lhs */;
	genCtx.builder().CreateStore(rhsValue, lhsAddress);
}

void LLVMCodegen::gen(ReturnStmt& n) {
	// generate code for the return value expression
	if (auto* rv = n.returnValue()) {
		rv->agen(*this);
	}

	// create the return instruction
	llvm::Value* retValue = /* get the value from return expression */;
	genCtx.builder().CreateRet(retValue);
}

void LLVMCodegen::gen(ProcCall& n) {
	(void)n;
}

void LLVMCodegen::gen(BreakStmt& n) {
	(void)n;
}

void LLVMCodegen::gen(ContinueStmt& n) {
	(void)n;
}

void LLVMCodegen::gen(IfStmt& n) {
	(void)n;
}

void LLVMCodegen::gen(LoopStmt& n) {
	(void)n;
}

void LLVMCodegen::gen(IdLVal& n) {
	(void)n;
}

void LLVMCodegen::gen(StringLiteralLVal& n) {
	(void)n;
}

void LLVMCodegen::gen(IndexLVal& n) {
	(void)n;
}

void LLVMCodegen::gen(IntConst& n) {
	// create llvm const int value
	value = genCtx.builder().getInt32(n.getValue());
}

void LLVMCodegen::gen(CharConst& n) {
	// create llvm const char value
	value = genCtx.builder().getInt8(n.getValue());
}

void LLVMCodegen::gen(TrueConst& n) {
	// true const = i8 1
	value = genCtx.builder().getInt8(1);
}

void LLVMCodegen::gen(FalseConst& n) {
	// false const = i8 0
	value = genCtx.builder().getInt8(0);
}

void LLVMCodegen::gen(LValueExpr& n) {
	// get address of the lvalue
	llvm::Value* addr = /* get address from lvalue */;
	auto elemType = genCtx.getLLVMType(*n.type());
	value = genCtx.builder().CreateLoad(elemType, addr);
}

void LLVMCodegen::gen(ParenExpr& n) {
	(void)n;
}

void LLVMCodegen::gen(FuncCall& n) {
	(void)n;
}

void LLVMCodegen::gen(UnaryExpr& n) {
	(void)n;
}

void LLVMCodegen::gen(BinaryExpr& n) {
	(void)n;
}

void LLVMCodegen::gen(ExprCond& n) {
	(void)n;
}

void LLVMCodegen::gen(ParenCond& n) {
	(void)n;
}

void LLVMCodegen::gen(NotCond& n) {
	(void)n;
}

void LLVMCodegen::gen(BinaryCond& n) {
	(void)n;
}

void LLVMCodegen::gen(RelCond& n) {
	(void)n;
}

llvm::Value* LLVMCodegen::genAddress(Lval& lv) {
	(void)lv;
	return nullptr;
}

llvm::Value* LLVMCodegen::genExpr(Expr& e) {
	e.agen(*this);
	return value;
}
