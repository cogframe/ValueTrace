#pragma once

#include "valuetrace/Slice.h"

#include <filesystem>
#include <string>
#include <vector>

namespace valuetrace {

struct ParameterQuery {
    std::string function;
    std::vector<unsigned> parameters;
    unsigned upDepth = 1;
    unsigned downDepth = 1;
    std::size_t maxContexts = 4000;
    std::size_t maxNodes = 100000;
    std::vector<std::string> parameterNames;
    bool selectReturn = false;
};

struct ImportedProgram {
    DependencyGraph graph;
    SliceQuery query;
    std::vector<std::string> diagnostics;
    std::vector<std::string> selections;
};

// Parses and verifies .ll or .bc. Owns LLVM objects only during import; the
// returned graph contains no references into the LLVM module.
ImportedProgram readParameterGraph(const std::filesystem::path& file,
                                   const ParameterQuery& query);
ImportedProgram readParameterGraph(const std::vector<std::filesystem::path>& files,
                                   const ParameterQuery& query);

} // namespace valuetrace
