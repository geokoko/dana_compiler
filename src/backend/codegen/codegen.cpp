#include "codegen.hpp"

void LLVMCodegen::gen(Program& n) {

}

void LLVMCodegen::gen(AssignStmt& n) {
	// generate code for the right-hand side expression
	n.rhs->agen(*this);
	
	// generate code for the left-hand side lvalue
	n.lhs->agen(*this);
	
	// perform the assignment
	llvm::Value* rhsValue = /* get the value from rhs */;
	llvm::Value* lhsAddress = /* get the address from lhs */;
	genCtx.builder().CreateStore(rhsValue, lhsAddress);
}

void LLVMCodegen::gen(ReturnStmt& n) {
	// generate code for the return value expression
	n.value->agen(*this);
	
	// create the return instruction
	llvm::Value* retValue = /* get the value from return expression */;
	genCtx.builder().CreateRet(retValue);
}

void LLVMCodegen::gen(LValueExpr& n) {
	// get address of the lvalue
	llvm::Value* addr = /* get address from lvalue */;
	auto elemType = genCtx.getLLVMType(*n.type());
	value = genCtx.builder().CreateLoad(elemType, addr);
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


