#pragma once

#include "valuetrace/LlvmReader.h"

namespace valuetrace {

enum class SolveStatus { Sat, Unsat, Unknown };
enum class NoWrapSemantics { UbFree, Wrap };

struct ReachabilityQuery {
    ParameterQuery target = [] { ParameterQuery query; query.downDepth = 0; return query; }();
    std::string comparison = "eq";
    std::string value;
    unsigned timeoutMs = 5000;
    std::size_t maxStates = 2000;
    std::size_t maxSteps = 50000;
    std::vector<std::string> externalInputSources = {"rand"};
    std::optional<unsigned> randMaxOverride;
    bool randUnbounded = false;
    NoWrapSemantics noWrapSemantics = NoWrapSemantics::UbFree;
    std::string selectedPointerFunction;
    std::vector<unsigned> selectedPointerArguments;
};

struct ReachabilityResult {
    SolveStatus status = SolveStatus::Unknown;
    std::string reason;
    std::vector<WitnessValue> inputs;
    std::vector<std::string> path;
    std::vector<std::string> constraints;
    std::vector<std::string> diagnostics;
    std::vector<std::string> unmodeledDependencies;
    std::vector<std::string> overapproximatedDependencies;
    std::vector<std::string> discardedDependencies;
    std::vector<std::string> unexploredPaths;
    std::vector<std::string> selections;
    std::string targetValue;
    std::size_t candidatesChecked = 0;
    bool upstreamBoundary = false;
    // Populated by the internal operation-risk query, not entry-value queries.
    std::vector<WitnessValue> operands;
    std::vector<WitnessValue> randCalls;
    std::vector<std::string> freeValues;
    std::vector<std::string> memoryEvidence;
    std::string randRangeBasis;
    bool resultIsPoison = false;
    bool noDefinedResult = false;
};

// Checks a single scalar integer parameter at function entry, under defined IR
// semantics and the selected caller scope. This is not a whole-program verdict.
ReachabilityResult solveParameterReachability(const std::filesystem::path& file,
                                              const ReachabilityQuery& query);
ReachabilityResult solveParameterReachability(const std::vector<std::filesystem::path>& files,
                                              const ReachabilityQuery& query);

} // namespace valuetrace
