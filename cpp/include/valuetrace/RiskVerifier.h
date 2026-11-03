#pragma once

#include "valuetrace/PathSolver.h"

namespace valuetrace {

struct VerificationOptions {
    unsigned timeoutMs = 5000; // Total verification batch, including import.
    std::size_t maxStates = 2000; // Per operation/reason query.
    std::size_t maxSteps = 50000;
    std::vector<std::string> externalInputSources = {"rand"}; // Explicitly declared symbolic sources.
    std::optional<unsigned> randMaxOverride;
    bool randUnbounded = false;
    NoWrapSemantics noWrapSemantics = NoWrapSemantics::UbFree;
};

struct RiskVerificationReport {
    ImportedProgram program;
    SliceResult slice;
    std::size_t candidates = 0;
    std::size_t solverChecks = 0;
};

// Verifies all candidate reasons in the selected slice before display filters.
// SAT remains boundary-triggerable until an actual source closure is proved.
RiskVerificationReport verifyArithmeticRisks(const std::filesystem::path& file,
                                            const ParameterQuery& scope,
                                            const VerificationOptions& options = {});
RiskVerificationReport verifyArithmeticRisks(const std::vector<std::filesystem::path>& files,
                                            const ParameterQuery& scope,
                                            const VerificationOptions& options = {});

} // namespace valuetrace
