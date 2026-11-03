#include "valuetrace/LlvmReader.h"
#include "valuetrace/ParameterSelection.h"

#include <iostream>
#include <stdexcept>

using namespace valuetrace;

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

template<class Action>
void failsWith(Action action, const std::string& reason) {
    try { action(); }
    catch (const std::exception& error) {
        require(std::string(error.what()).find(reason) != std::string::npos,
                "expected " + reason + ", got: " + error.what());
        return;
    }
    throw std::runtime_error("expected failure: " + reason);
}

ParameterQuery byName(std::string function, std::string parameter) {
    ParameterQuery query;
    query.function = std::move(function);
    query.parameterNames.push_back(std::move(parameter));
    return query;
}

const ParameterCandidate& parameter(const FunctionCandidate& function, const std::string& name) {
    for (const auto& candidate : function.parameters) if (candidate.name == name) return candidate;
    throw std::runtime_error("source parameter missing: " + name);
}

void checkNamedChain(const std::filesystem::path& ll, const std::filesystem::path& bc,
                     const std::filesystem::path& nodebug) {
    auto query = byName("B", "count");
    const auto named = readParameterGraph(ll, query);
    const auto numbered = readParameterGraph(ll, {"B", {0}, 1, 1});
    require(named.query.parameters == numbered.query.parameters, "name and numeric selections differ");
    const auto binary = readParameterGraph(bc, query);
    require(binary.query.parameters == named.query.parameters, "bc name mapping differs");
    require(!named.selections.empty() && named.selections.front().find("'count' -> LLVM argument 0") != std::string::npos, "mapping evidence not reported");
    query.parameterNames = {"count", "extra", "count"};
    require(readParameterGraph(ll, query).query.parameters.size() == 2, "named group deduplication failed");
    failsWith([&] { readParameterGraph(nodebug, byName("B", "count")); }, "[debug-info-missing]");
    failsWith([&] { readParameterGraph(ll, byName("B", "Count")); }, "[parameter-name-not-found]");
}

void checkCppCandidates(const std::filesystem::path& file) {
    const auto overloads = inspectParameters(file, "overloaded");
    require(overloads.size() == 2, "C++ overload candidates missing");
    for (const auto& function : overloads) {
        require(!function.source.file.empty() && function.source.line != 0, "candidate source location missing");
        require(parameter(function, "count").traceable, "overload source name mapping missing");
        require(readParameterGraph(file, byName(function.symbol, "count")).query.parameters.size() == 1, "explicit overload selection failed");
    }
    failsWith([&] { readParameterGraph(file, byName("overloaded", "count")); }, "[ambiguous-function]");

    const auto method = inspectParameters(file, "Worker::process");
    require(method.size() == 1, "member function candidate missing");
    const auto& self = parameter(method.front(), "this");
    const auto& count = parameter(method.front(), "count");
    require(self.implicit && !self.traceable && self.status == "unsupported-type", "this must be an unsupported implicit pointer");
    require(self.llvmIndex == 0 && count.llvmIndex == 1, "this offset not mapped from debug values");
    const auto named = readParameterGraph(file, byName("Worker::process", "count"));
    const auto numeric = readParameterGraph(file, {method.front().symbol, {1}, 1, 1});
    require(named.query.parameters == numeric.query.parameters, "count incorrectly selected this");
    failsWith([&] { readParameterGraph(file, byName("Worker::process", "this")); }, "[unsupported-type]");

    const auto hiddenReturn = inspectParameters(file, "make_large");
    require(parameter(hiddenReturn.front(), "count").llvmIndex == 1, "source ordinal was confused with LLVM index after sret");
    require(readParameterGraph(file, byName("make_large", "count")).query.parameters.size() == 1, "sret shifted parameter not selectable");
    failsWith([&] { inspectParameters(file, "Inlined"); }, "[inline-evidence]");
    failsWith([&] { inspectParameters(file, "AbsentName"); }, "cause is not established");
}

void checkMissingMappings(const std::filesystem::path& file) {
    for (const auto& [function, reason] : {
             std::pair{"computed", "[non-argument-location]"},
             std::pair{"expression", "[transformed-mapping]"},
             std::pair{"fragmented", "[fragmented-mapping]"},
             std::pair{"ambiguous", "[ambiguous-mapping]"},
             std::pair{"missing_map", "[mapping-missing]"},
             std::pair{"addressed", "[address-mapping-unsupported]"},
             std::pair{"aggregate", "[unsupported-source-type]"}}) {
        failsWith([&] { readParameterGraph(file, byName(function, "count")); }, reason);
        require(!readParameterGraph(file, {function, {0}, 0, 0}).query.parameters.empty(),
                "explicit LLVM-index fallback must not depend on missing source mapping");
    }
    const auto shadow = inspectParameters(file, "shadowed");
    require(parameter(shadow.front(), "count").llvmIndex == 0, "local variable shadow replaced formal parameter");
    require(readParameterGraph(file, byName("shadowed", "count")).query.parameters.size() == 1, "shadowed parameter selection failed");
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 6) throw std::invalid_argument("expected chain ll, bc, nodebug ll, named C++ ll and mapping fixture");
        checkNamedChain(argv[1], argv[2], argv[3]);
        checkCppCandidates(argv[4]);
        checkMissingMappings(argv[5]);
        std::cout << "Parameter names, overloads, this/sret and evidence-specific failures passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
