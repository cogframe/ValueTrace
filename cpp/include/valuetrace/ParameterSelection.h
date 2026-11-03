#pragma once

#include "valuetrace/DependencyGraph.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace valuetrace {

struct ParameterCandidate {
    std::string name;
    std::string sourceType;
    std::string irType;
    std::optional<unsigned> llvmIndex;
    bool implicit = false;
    bool traceable = false;
    std::string status;
    std::string detail;
};

struct FunctionCandidate {
    std::string symbol;
    std::string signature;
    SourceLocation source;
    std::vector<ParameterCandidate> parameters;
    std::vector<std::string> notes;
    bool debugInfoAvailable = false;
    std::vector<unsigned> scalarIntegerArguments;
    std::string irInput;
};

// Read-only discovery. Does not pick one overload or guess missing source names.
std::vector<FunctionCandidate> inspectParameters(const std::filesystem::path& file,
                                                const std::string& function);
std::vector<FunctionCandidate> inspectParameters(const std::vector<std::filesystem::path>& files,
                                                const std::string& function);

} // namespace valuetrace
