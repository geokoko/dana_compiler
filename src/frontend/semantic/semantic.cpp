#include "semantic.hpp"

#include "semantic_pass.hpp"
#include "control_flow.hpp"

void runSemanticAnalysis(Program& program, SemContext& context) {
	// Phase 1: Combined symbol declaration and type checking pass
	// Builds the symbol table, resolves types, declares variables and functions,
	// and validates types of expressions, assignments, and function calls
	runSemanticPass(program, context);

	// Phase 2: Control flow pass
	// Validates return paths, break/continue usage, and control flow
	runControlFlowPass(program, context);
}

