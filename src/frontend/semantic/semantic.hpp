#pragma once

class Program;
class SemContext;

void runSemanticPass(Program& program, SemContext& context);
void runControlFlowPass(Program& program, SemContext& context);
