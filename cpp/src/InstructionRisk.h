#pragma once

#include "valuetrace/PathSolver.h"
#include <set>
#include <optional>

namespace llvm { class Module; class Instruction; class CallBase; }

namespace valuetrace::execution {

using CallContext = std::vector<const llvm::CallBase*>;

struct InstructionRiskTarget {
    const llvm::Instruction* instruction;
    ArithmeticRiskReason reason;
    CallContext context;
    std::set<CallContext> allowedContexts;
    std::optional<unsigned> closedUpDepth;
    std::string closureReason;
};

ReachabilityResult solveInstructionRisk(llvm::Module& module, const ReachabilityQuery& limits,
                                       const InstructionRiskTarget& target);

} // namespace valuetrace::execution
