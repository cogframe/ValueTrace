#include "SolveOutput.h"
#include <iostream>

namespace valuetrace::cli {

int printSolveResult(const ReachabilityResult& result) {
    const auto status = result.status == SolveStatus::Sat ? "SAT" : result.status == SolveStatus::Unsat ? "UNSAT" : "UNKNOWN";
    std::cout << status << '\n' << result.reason << '\n';
    std::cout << "Scope: selected function-entry parameter and bounded upstream caller paths. Not a whole-program safety verdict.\n";
    if (result.upstreamBoundary) std::cout << "Upstream depth boundary: caller constraints outside the scope were not included.\n";
    for (const auto& selection : result.selections) std::cout << "Mapping: " << selection << '\n';
    for (const auto& input : result.inputs) {
        std::cout << "Boundary input " << input.name << " i" << input.bitWidth
                  << " unsigned=" << input.unsignedValue << " signed=" << input.signedValue << '\n';
    }
    if (!result.targetValue.empty()) std::cout << "Target bit pattern (unsigned decimal): " << result.targetValue << '\n';
    for (const auto& step : result.path) std::cout << "Path: " << step << '\n';
    for (const auto& constraint : result.constraints) std::cout << "Constraint: " << constraint << '\n';
    for (const auto& diagnostic : result.diagnostics) {
        if (diagnostic.starts_with("Potential undefined behavior:"))
            std::cout << "Undefined behavior candidate:" << diagnostic.substr(sizeof("Potential undefined behavior:") - 1) << '\n';
        else std::cout << "Model note: " << diagnostic << '\n';
    }
    std::cout << "Target contexts checked: " << result.candidatesChecked << '\n';
    return result.status == SolveStatus::Unknown ? 1 : 0;
}

} // namespace valuetrace::cli
