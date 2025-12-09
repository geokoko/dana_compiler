#include "codegen.hpp"

/* ========== Loops ========== */

void LLVMCodegen::gen(LoopStmt& n) {
	auto* function = genCtx.builder().GetInsertBlock() ? genCtx.builder().GetInsertBlock()->getParent() : nullptr;
	if (!function) {
		value = nullptr;
		return;
	}

	std::string prefix = n.loopLabel() ? *n.loopLabel() : "loop";
	auto* bodyBB = llvm::BasicBlock::Create(genCtx.llvmContext(), prefix + ".body", function);
	auto* exitBB = llvm::BasicBlock::Create(genCtx.llvmContext(), prefix + ".end", function);

	auto* currentBB = genCtx.builder().GetInsertBlock();
	if (currentBB && !currentBB->getTerminator()) {
		genCtx.builder().CreateBr(bodyBB);
	}

	genCtx.builder().SetInsertPoint(bodyBB);
	if (auto* fi = genCtx.currentFrameInfo()) {
		fi->pushLoop(exitBB, bodyBB);
	}
	if (auto* body = n.loopBody()) {
		body->agen(*this);
	}
	if (!bodyBB->getTerminator()) {
		genCtx.builder().CreateBr(bodyBB);
	}
	if (auto* fi = genCtx.currentFrameInfo()) {
		fi->popLoop();
	}

	genCtx.builder().SetInsertPoint(exitBB);
	value = nullptr;
}

/* ========== Break / Continue ========== */

void LLVMCodegen::gen(BreakStmt& n) {
	(void)n;
	auto* frameInfo   = genCtx.currentFrameInfo();
	auto* breakTarget = frameInfo ? frameInfo->currentBreakTarget() : nullptr;
	if (!breakTarget) {
		value = nullptr;
		return;
	}
	auto* currentBB  = genCtx.builder().GetInsertBlock();
	auto* parentFn   = currentBB ? currentBB->getParent() : nullptr;

	genCtx.builder().CreateBr(breakTarget);
	if (parentFn) {
		auto* contBB = llvm::BasicBlock::Create(genCtx.llvmContext(), "break.cont", parentFn);
		genCtx.builder().SetInsertPoint(contBB);
	}
	value = nullptr;
}

void LLVMCodegen::gen(ContinueStmt& n) {
	(void)n;
	auto* frameInfo      = genCtx.currentFrameInfo();
	auto* continueTarget = frameInfo ? frameInfo->currentContinueTarget() : nullptr;
	if (!continueTarget) {
		value = nullptr;
		return;
	}
	auto* currentBB  = genCtx.builder().GetInsertBlock();
	auto* parentFn   = currentBB ? currentBB->getParent() : nullptr;

	genCtx.builder().CreateBr(continueTarget);
	if (parentFn) {
		auto* contBB = llvm::BasicBlock::Create(genCtx.llvmContext(), "continue.cont", parentFn);
		genCtx.builder().SetInsertPoint(contBB);
	}
	value = nullptr;
}


