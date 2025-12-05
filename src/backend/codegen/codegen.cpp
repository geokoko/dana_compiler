#include "codegen.hpp"

#include <memory>
#include <vector>

void LLVMCodegen::gen(Program& n) {
	if (auto* def = n.definition()) {
		def->agen(*this);
	}
}

void LLVMCodegen::gen(FuncDef& n) {
	/* Resolve function symbol */
	auto* header = n.funcHeader();
	auto* funcSym = header ? header->symbol() : nullptr;
	if (!funcSym) {
		value = nullptr;
		return;
	}

	// Get or create FrameInfo for this function
	auto* frameInfo = genCtx.createFrameInfo(funcSym);
	frameInfo->setFunctionSymbol(funcSym);

	/* Determine parent frame type for static link */
	llvm::StructType* parentFrameTy = nullptr;
	if (auto* parentSym = funcSym->definingFunc()) {
		if (const auto* parentInfo = genCtx.getFrameInfo(parentSym)) {
			parentFrameTy = parentInfo->frameType();
		}
	}

	/* Build frame layout */
	std::vector<llvm::Type*> paramTys;
	std::vector<llvm::Type*> localTys;
	// build static link field 0
	llvm::Type* staticLinkTy = parentFrameTy ? llvm::PointerType::getUnqual(parentFrameTy)
											 : llvm::Type::getInt8PtrTy(genCtx.llvmContext());
	// build parameter and local variable fields
	std::size_t fieldIndex = 1; // 0 reserved for static link

	/* Collect parameters and local variables into frame */
	for (const auto& p : funcSym->getParams()) {
		// add parameter slots and capture them
		paramTys.push_back(genCtx.getLLVMType(*p->getType(), /*forParam=*/true));
		frameInfo->captureVar(p.get(), fieldIndex++);
	}
	for (auto& def : n.localDefs()) {
		if (auto* var = dynamic_cast<VarDef*>(def.get())) {
			for (auto* sym : var->symbols()) {
				// add local variable slots and capture them
				localTys.push_back(genCtx.getLLVMType(*sym->getType()));
				frameInfo->captureVar(sym, fieldIndex++);
			}
		}
	}

	if (!frameInfo->frameType()) {
		frameInfo->setFrameType(llvm::StructType::create(genCtx.llvmContext(), funcSym->getName() + ".frame"));
	}
	auto* frameTy = frameInfo->frameType();

	// set frame body: static link + params + locals
	std::vector<llvm::Type*> frameFields;
	frameFields.reserve(1 + paramTys.size() + localTys.size());
	frameFields.push_back(staticLinkTy);
	frameFields.insert(frameFields.end(), paramTys.begin(), paramTys.end());
	frameFields.insert(frameFields.end(), localTys.begin(), localTys.end());
	frameTy->setBody(frameFields, /*isPacked=*/false);
	
	/* Create LLVM function type */
	const auto* funcTy = static_cast<const FuncType*>(funcSym->getType().get());

	// get & set return type
	std::vector<llvm::Type*> stLinkAndParamTys;

	if (funcSym->definingFunc()) {
		// add static link as first param
		stLinkAndParamTys.push_back(staticLinkTy);
	}
	stLinkAndParamTys.insert(stLinkAndParamTys.end(), paramTys.begin(), paramTys.end());
	llvm::Type* retTy = genCtx.getLLVMType(*funcTy->returnType());
	llvm::FunctionType* fnTy = llvm::FunctionType::get(retTy, stLinkAndParamTys, /*isVarArg=*/false);

	/* Create LLVM function and bind to symbol */
	// Check if function already exists (for forward declarations)
	llvm::Function* llvmFunc = genCtx.lookupFunction(funcSym);
	if (!llvmFunc) {
		llvmFunc = llvm::Function::Create(fnTy, llvm::Function::ExternalLinkage, funcSym->getName(), &genCtx.llvmModule());
		genCtx.bindFunction(funcSym, llvmFunc);
	}

	frameInfo->setLLVMFunction(llvmFunc);

	/* Create LLVM function basic block and set insertion point */
	auto* entry = llvm::BasicBlock::Create(genCtx.llvmContext(), funcSym->getName() + ".entry", llvmFunc);
	genCtx.builder().SetInsertPoint(entry);
	/* Put allocas for frame and bind frame pointer */
	llvm::Value* framePtr = genCtx.builder().CreateAlloca(frameTy, nullptr, funcSym->getName() + ".frame");

	genCtx.enterFunction(funcSym, frameInfo, framePtr);
	
	/* Bind static link and parameters to frame fields */
	auto argIt = llvmFunc->arg_begin();
	
	if (funcSym->definingFunc()) {
		// bind static link
		argIt->setName("staticlink.arg");
		llvm::Value* staticLinkArg = &*argIt++;
		llvm::Value* staticLinkPtr = genCtx.builder().CreateStructGEP(frameTy, framePtr, 0, "staticlink.ptr");
		genCtx.builder().CreateStore(staticLinkArg, staticLinkPtr);
	} else {
		// no static link, bind null
		llvm::Value* staticLinkPtr = genCtx.builder().CreateStructGEP(frameTy, framePtr, 0, "staticlink.ptr");
		llvm::Value* nullPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(staticLinkTy));		
		genCtx.builder().CreateStore(nullPtr, staticLinkPtr);
	}

	// bind parameters
	for (const auto& p : funcSym->getParams()) {
		auto idxOpt = frameInfo->getCapturedVarIndex(p.get());
		assert(idxOpt.has_value());
		std::size_t idx = *idxOpt;
		llvm::Value* paramArg = &*argIt++;
		llvm::Value* paramPtr = genCtx.builder().CreateStructGEP(frameTy, framePtr,
																idx, p->getName() + ".ptr");
		// store parameter into frame (call by value)
		genCtx.builder().CreateStore(paramArg, paramPtr);
		genCtx.bindValue(p.get(), paramPtr);
		// TODO: handle by-ref parameters
	}

	/* Bind local variables to function frame */
	for (auto& def : n.localDefs()) {
		if (auto* var = dynamic_cast<VarDef*>(def.get())) {
			for (auto* sym : var->symbols()) {
				auto idxOpt = frameInfo->getCapturedVarIndex(sym);
				assert(idxOpt.has_value());
				std::size_t idx = *idxOpt;
				llvm::Value* localPtr = genCtx.builder().CreateStructGEP(frameTy, framePtr, idx, sym->getName() + ".ptr");
				genCtx.bindValue(sym, localPtr);
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

void LLVMCodegen::gen(Block& n) {
	for (auto& stmt : n.statementsList()) {
		stmt->agen(*this);
	}
}

void LLVMCodegen::gen(SkipStmt& n) {
	// no-op statement: do nothing
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
		rhs->agen(*this);
		rhsValue = value;
	}
	if (auto* lhs = n.left()) {
		lhs->agen(*this);
		lhsAddress = value;
	}

	if (rhsValue && lhsAddress) {
		genCtx.builder().CreateStore(rhsValue, lhsAddress);
	}
	value = nullptr;
}

void LLVMCodegen::gen(ReturnStmt& n) {
	// Simple return handling: return value if present, else void
	if (!genCtx.currentFunc()) {
		value = nullptr;
		return;
	}
	auto* fnSym = genCtx.currentFunc();
	auto* sig = static_cast<const FuncType*>(fnSym->getType().get());
	auto* retTy = sig ? genCtx.getLLVMType(*sig->returnType()) : genCtx.builder().getVoidTy();

		if (auto* expr = n.returnValue()) {
			expr->agen(*this);
			auto* v = value;
			genCtx.builder().CreateRet(v ? v : llvm::UndefValue::get(retTy));
	} else {
		if (retTy->isVoidTy()) genCtx.builder().CreateRetVoid();
		else genCtx.builder().CreateRet(llvm::UndefValue::get(retTy));
	}
	value = nullptr;
}

void LLVMCodegen::gen(ProcCall& n) {
	(void)n;
	value = nullptr;
}

void LLVMCodegen::gen(BreakStmt& n) {
	(void)n;
	value = nullptr;
}

void LLVMCodegen::gen(ContinueStmt& n) {
	(void)n;
	value = nullptr;
}

void LLVMCodegen::gen(IfStmt& n) {
	(void)n;
	value = nullptr;
}

void LLVMCodegen::gen(LoopStmt& n) {
	(void)n;
	value = nullptr;
}

void LLVMCodegen::gen(IdLVal& n) {
	value = genCtx.lookupValue(n.symbol());
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
	llvm::Value* addr = nullptr;
	if (auto* lv = n.lvalue()) {
		lv->agen(*this);
		addr = value;
	}
	if (!addr) {
		value = nullptr;
		return;
	}
	auto elemType = genCtx.getLLVMType(*n.type());
	value = genCtx.builder().CreateLoad(elemType, addr);
}

void LLVMCodegen::gen(ParenExpr& n) {
	if (auto* inner = n.innerExpr()) {
		inner->agen(*this);
	} else {
		value = nullptr;
	}
}

void LLVMCodegen::gen(FuncCall& n) {
	(void)n;
	value = nullptr;
}

void LLVMCodegen::gen(UnaryExpr& n) {
	llvm::Value* operand = nullptr;
	if (auto* expr = n.operandExpr()) {
		expr->agen(*this);
		operand = value;
	}
	if (!operand) {
		value = nullptr;
		return;
	}

	switch (n.opKind()) {
		case UnOp::Plus:
			value = operand;
			break;
		case UnOp::Minus:
			value = genCtx.builder().CreateNeg(operand);
			break;
		case UnOp::Not:
			value = genCtx.builder().CreateNot(operand);
			break;
	}
}

void LLVMCodegen::gen(BinaryExpr& n) {
	llvm::Value* lhs = nullptr;
	llvm::Value* rhs = nullptr;
	if (auto* left = n.leftExpr()) {
		left->agen(*this);
		lhs = value;
	}
	if (auto* right = n.rightExpr()) {
		right->agen(*this);
		rhs = value;
	}
	if (!lhs || !rhs) {
		value = nullptr;
		return;
	}
	switch (n.opKind()) {
		case BinOp::Add:
			value = genCtx.builder().CreateAdd(lhs, rhs);
			break;
		case BinOp::Sub:
			value = genCtx.builder().CreateSub(lhs, rhs);
			break;
		case BinOp::Mul:
			value = genCtx.builder().CreateMul(lhs, rhs);
			break;
		case BinOp::Div:
			value = genCtx.builder().CreateSDiv(lhs, rhs);
			break;
		case BinOp::Mod:
			value = genCtx.builder().CreateSRem(lhs, rhs);
			break;
		case BinOp::AndBits:
			value = genCtx.builder().CreateAnd(lhs, rhs);
			break;
		case BinOp::OrBits:
			value = genCtx.builder().CreateOr(lhs, rhs);
			break;
		default:
			value = nullptr;
			break;
	}
}

void LLVMCodegen::gen(ExprCond& n) {
	auto* expr = n.expression();
	if (!expr) {
		value = nullptr;
		return;
	}
	expr->agen(*this);
	auto* condVal = value;
	if (!condVal) {
		value = nullptr;
		return;
	}
	if (condVal->getType()->isIntegerTy(1)) {
		value = condVal;
		return;
	}
	if (condVal->getType()->isIntegerTy()) {
		auto* zero = llvm::ConstantInt::get(condVal->getType(), 0);
		value = genCtx.builder().CreateICmpNE(condVal, zero, "boolcast");
		return;
	}
	value = nullptr;
}

void LLVMCodegen::gen(ParenCond& n) {
	if (auto* inner = n.conditionExpr()) {
		inner->agen(*this);
	} else {
		value = nullptr;
	}
}

void LLVMCodegen::gen(NotCond& n) {
	auto* inner = n.conditionExpr();
	if (!inner) {
		value = nullptr;
		return;
	}
	inner->agen(*this);
	auto* condVal = value;
	if (!condVal) {
		return;
	}
	if (!condVal->getType()->isIntegerTy(1)) {
		if (condVal->getType()->isIntegerTy()) {
			auto* zero = llvm::ConstantInt::get(condVal->getType(), 0);
			condVal = genCtx.builder().CreateICmpNE(condVal, zero, "boolcast");
		} else {
			value = nullptr;
			return;
		}
	}
	value = genCtx.builder().CreateNot(condVal, "nottmp");
}

void LLVMCodegen::gen(BinaryCond& n) {
	// TODO: short-circuiting AND/OR using basic blocks and CreateCondBr
	(void)n;
}

void LLVMCodegen::gen(RelCond& n) {
	// TODO: evaluate lhs/rhs via genExpr, cast to matching integer types, then CreateICmp*
	(void)n;
}
