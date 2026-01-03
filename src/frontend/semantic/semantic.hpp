#pragma once

class Program;
class SemContext;

// Individual semantic analysis passes
#include "semantic_pass.hpp"
#include "control_flow.hpp"

// Convenience function that runs all semantic passes in order
void runSemanticAnalysis(Program& program, SemContext& context);