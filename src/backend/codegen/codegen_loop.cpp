#include "codegen.hpp"

/* ========== Loops ========== */

void Codegen::visit(LoopStmt& n) {
	auto* curBB = genCtx.builder().GetInsertBlock();
	auto* function = curBB ? genCtx.builder().GetInsertBlock()->getParent() : nullptr;
	if (!function) {
		value = nullptr;
		return;
	}

	std::string prefix = n.loopLabel() ? *n.loopLabel() : "loop";
	auto* bodyBB = llvm::BasicBlock::Create(genCtx.llvmContext(), prefix + ".body", function);
	auto* exitBB = llvm::BasicBlock::Create(genCtx.llvmContext(), prefix + ".end", function);

	if (curBB && !curBB->getTerminator()) {
		genCtx.builder().CreateBr(bodyBB);
	}

	genCtx.builder().SetInsertPoint(bodyBB);
	genCtx.pushLoop(exitBB, bodyBB);

	if (auto* body = n.loopBody()) {
		body->accept(*this);
	}

	// Ensure jump back to body from possible other blocks
	auto* jumpBlock = genCtx.builder().GetInsertBlock();
	if (!jumpBlock->getTerminator()) {
		genCtx.builder().CreateBr(bodyBB);
	}
	
	genCtx.popLoop();

	// Continue from this basic block
	genCtx.builder().SetInsertPoint(exitBB);
	value = nullptr;
}

/* ========== Break / Continue ========== */

void Codegen::visit(BreakStmt& n) {
	(void)n;
	auto* breakTarget = genCtx.currentBreakTarget();
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
		// Must have a terminator
		genCtx.builder().CreateUnreachable();
	}
	value = nullptr;
}

void Codegen::visit(ContinueStmt& n) {
	(void)n;
	auto* continueTarget = genCtx.currentContinueTarget();
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
		// Must have a terminator
		genCtx.builder().CreateUnreachable();
	}
	value = nullptr;
}