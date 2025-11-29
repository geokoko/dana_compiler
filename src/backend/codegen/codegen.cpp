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
	auto* funcSym = header ? header->symbol() : nullptr;
	if (!funcSym) {
		value = nullptr;
		return;
	}

	auto* sig = static_cast<const FuncType*>(funcSym->getType().get());

	// Build function parameter types (prepend static link if needed)
	std::vector<llvm::Type*> paramTypes;
	auto* frameInfo = genCtx.frameInfo(funcSym);
	const bool needsStaticLink = frameInfo && frameInfo->hasFrame && funcSym->definingFunc();
	if (needsStaticLink) {
		// TODO: use the defining function's frame pointer type here
		paramTypes.push_back(/* static link pointer type */);
	}
	for (const auto& p : funcSym->getParams()) {
		paramTypes.push_back(genCtx.getLLVMType(*p->getType(), /*forParam=*/true));
	}

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

	llvm::Value* framePtr = nullptr;
	if (frameInfo && frameInfo->hasFrame) {
		// TODO: allocate the frame struct, store static link in field 0, and keep the resulting pointer
		// framePtr = allocaBuilder.CreateAlloca(/* frame struct type */);
	}

	genCtx.enterFunction(funcSym, framePtr);

	// Bind the static link argument if present
	auto argIt = fn->arg_begin();
	if (needsStaticLink && argIt != fn->arg_end()) {
		argIt->setName("static_link");
		// TODO: decide whether to store static link inside framePtr or keep as-is
		++argIt;
	}

	// bind param symbols, allocate memory and add them as function arguments to LLVM func
	for (std::size_t i = 0; i < funcSym->getParams().size() && argIt != fn->arg_end(); ++i, ++argIt) {
		auto& arg = *argIt;
		arg.setName(funcSym->getParams()[i]->getName());
		const bool byRef = funcSym->getParams()[i]->getPass() == Symbol::ParamPass::BY_REF;
		if (byRef) {
			// TODO: if captured, store pointer in frame; otherwise bind directly
			genCtx.bindValue(funcSym->getParams()[i].get(), &arg);
		} else {
			// TODO: decide whether param lives in frame or stack; default to stack alloca
			auto* slot = allocaBuilder.CreateAlloca(arg.getType(), nullptr, arg.getName() + ".addr");
			genCtx.builder().CreateStore(&arg, slot);
			genCtx.bindValue(funcSym->getParams()[i].get(), slot);
		}
	}

	// local defs handling
	for (auto& def : n.localDefs()) {
		if (auto* var = dynamic_cast<VarDef*>(def.get())) {
			const auto& syms = var->symbols();
			for (auto* sym : syms) {
				if (frameInfo && frameInfo->fieldIndex.count(sym)) {
					// TODO: GEP into framePtr using fieldIndex to bind captured locals
				} else {
					auto* ty = genCtx.getLLVMType(*sym->getType());
					// TODO: this path is for non-captured locals; stack allocas are fine
					auto* slot = allocaBuilder.CreateAlloca(ty, nullptr, sym->getName());
					genCtx.bindValue(sym, slot);
				}
			}
		}
	}

	// handle func body
	if (n.funcBody()) {
		n.funcBody()->agen(*this);
	}

	// if no terminator for current block, set return type
	if (!entry->getTerminator()) {
		// TODO: pick default return: void or undef of retTy
		if (retTy->isVoidTy()) genCtx.builder().CreateRetVoid();
		else genCtx.builder().CreateRet(llvm::UndefValue::get(retTy));
	}

	genCtx.leaveFunction();
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
	llvm::Value* rhsValue = nullptr;
	llvm::Value* lhsAddress = nullptr;

	if (auto* rhs = n.right()) {
		// TODO: call genExpr(rhs) to produce the value
		rhsValue = /* genExpr(*rhs) */;
	}
	if (auto* lhs = n.left()) {
		// TODO: call genAddress(lhs) to produce the destination pointer
		lhsAddress = /* genAddress(*lhs) */;
	}

	// TODO: handle any needed type conversions 
	// genCtx.builder().CreateStore(rhsValue, lhsAddress);
	value = nullptr;
}

void LLVMCodegen::gen(ReturnStmt& n) {

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
}

void LLVMCodegen::gen(LoopStmt& n) {
}

void LLVMCodegen::gen(IdLVal& n) {
	// TODO: prefer using getVarAddress with the bound symbol
	value = getVarAddress(n.symbol());
}

void LLVMCodegen::gen(StringLiteralLVal& n) {
	// TODO: materialize/lookup a global string constant and return its address
	(void)n;
	value = nullptr;
}

void LLVMCodegen::gen(IndexLVal& n) {
	// TODO: compute base address (call genAddress on base) and index value (genExpr)
	// TODO: GEP into element and set value to resulting address
	(void)n;
	value = nullptr;
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
	llvm::Value* addr = /* genAddress(*n.value()) */;
	// TODO: handle type casting/loading rules
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
	// TODO: evaluate operands	
	// // TODO: insert casts so both operands have the expected LLVM type (i32/i8)
	switch (n.getOp()) {
		case BinOp::Add:
			value =
			break;
		case BinOp::Sub:
			value = 
			break;
		case BinOp::Mul:
			value =
			break;
		case BinOp::Div:
			value = 
			break;
		case BinOp::Mod:
			value =
			break;
		case BinOp::AndBits:
			value =
			break;
		case BinOp::OrBits:
			value = 
			break;
		default:
			// TODO: handle any additional operators if added later
			value = nullptr;
			break;
	}
}

void LLVMCodegen::gen(ExprCond& n) {
	// TODO: call genExpr on the inner expression and ensure it is cast/compared to i1
	(void)n;
}

void LLVMCodegen::gen(ParenCond& n) {
	// TODO: just delegate to the wrapped condition
	(void)n;
}

void LLVMCodegen::gen(NotCond& n) {
	// TODO: emit the operand condition then CreateNot on the resulting i1/i8
	(void)n;
}

void LLVMCodegen::gen(BinaryCond& n) {
	// TODO: short-circuiting AND/OR using basic blocks and CreateCondBr
	(void)n;
}

void LLVMCodegen::gen(RelCond& n) {
	// TODO: evaluate lhs/rhs via genExpr, cast to matching integer types, then CreateICmp*
	(void)n;
}

llvm::Value* LLVMCodegen::genAddress(Lval& lv) {
	lv.agen(*this);
	// TODO: ensure l-value visitors leave 'value' as the address of the l-value
	return value;
}

llvm::Value* LLVMCodegen::genExpr(Expr& e) {
	e.agen(*this);
	return value;
}

llvm::Value* LLVMCodegen::getVarAddress(const Symbol* sym) {
	if (!sym) {
		return nullptr;
	}

	// TODO: implement address loading
	return genCtx.lookupValue(sym);
}

llvm::Value* LLVMCodegen::makeStaticLink(const FuncSymbol* callee) {
	if (!callee || !callee->definingFunc()) {
		return nullptr; // top-level function: no static link
	}
}
