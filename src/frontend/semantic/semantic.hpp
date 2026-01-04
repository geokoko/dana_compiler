// Convenience header that includes both semantic passes.
// Individual headers can also be included directly:
//   - semantic_pass.hpp  (symbol table, type checking)
//   - control_flow.hpp   (return/exit/break/continue validation)
#pragma once

#include "control_flow.hpp"
#include "semantic_pass.hpp"
