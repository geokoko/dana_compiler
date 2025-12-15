#include "optimizer.h"

#include <llvm/Transforms/Utils/Mem2Reg.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SCCP.h>
#include <llvm/Transforms/Scalar/GVN.h>

Optimizer::Optimizer() {
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
}

void Optimizer::optimize(llvm::Module& module, llvm::OptimizationLevel level) {
    llvm::ModulePassManager MPM;
    llvm::FunctionPassManager FPM;

	// Always promote memory to register
    FPM.addPass(llvm::PromotePass());
    
	// Add optimizations based on the requested level
	// O0: No optimizations
	// O1 and above: Basic optimizations
	// O2 and above: More aggressive optimizations
	// O3: Maximum optimizations
	// LLVM handles these levels internally
    if (level != llvm::OptimizationLevel::O0) {
        // Add Control Flow Simplification
        FPM.addPass(llvm::SimplifyCFGPass());

        // Add Instruction Combining (peephole optimizations & data flow)
        FPM.addPass(llvm::InstCombinePass());
    }

    // Add into the module manager
    MPM.addPass(llvm::createModuleToFunctionPassAdaptor(std::move(FPM)));

    // Run the default pipeline for the requested level
    if (level != llvm::OptimizationLevel::O0) {
        MPM.addPass(PB.buildPerModuleDefaultPipeline(level));
    }

    MPM.run(module, MAM);
}
