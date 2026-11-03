#pragma once

#include "valuetrace/DependencyGraph.h"
#include "IntegerExpressions.h"
#include <span>

namespace valuetrace::symbolic {

// This predicate intentionally does not include the target instruction's
// valid/no-wrap condition. Only the operand values and earlier path are valid.
z3::expr riskCondition(const llvm::Instruction& instruction, ArithmeticRiskReason reason,
                       std::span<const Value> operands);

} // namespace valuetrace::symbolic
