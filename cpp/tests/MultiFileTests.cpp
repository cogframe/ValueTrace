#include "valuetrace/ProgramInput.h"
#include "valuetrace/ParameterSelection.h"
#include "valuetrace/PathSolver.h"
#include "valuetrace/RiskVerifier.h"
#include "chain.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

// Native replay supplies this stub; analysis inputs deliberately omit its implementation.
extern "C" unsigned CrossMissing(unsigned value) { return value; }
using namespace valuetrace;
namespace {
void require(bool ok, const std::string& why) { if (!ok) throw std::runtime_error(why); }
ParameterQuery query(std::string name, unsigned up = 0, unsigned down = 0) {
    ParameterQuery q; q.function = std::move(name); q.parameterNames = {"value"}; q.upDepth = up; q.downDepth = down; return q;
}
const ArithmeticRisk& risk(const RiskVerificationReport& report, ArithmeticRiskReason reason) {
    for (NodeId id = 0; id < report.program.graph.size(); ++id)
        for (const auto& r : report.program.graph.node(id).arithmeticRisks)
            if (r.reason == reason && r.verification) return r;
    throw std::runtime_error("missing multifile verification");
}
bool hasRisk(const RiskVerificationReport& report, ArithmeticRiskReason reason) {
    for (NodeId id = 0; id < report.program.graph.size(); ++id)
        for (const auto& item : report.program.graph.node(id).arithmeticRisks)
            if (item.reason == reason && item.verification) return true;
    return false;
}
template<class Action> void reject(Action action, const std::string& expected) {
    try { action(); } catch (const std::exception& e) {
        require(std::string(e.what()).find(expected) != std::string::npos, "wrong failure: " + std::string(e.what())); return;
    }
    throw std::runtime_error("expected failure: " + expected);
}
std::vector<std::filesystem::path> units(const std::filesystem::path& directory) {
    return {directory / "c.ll", directory / "a.ll", directory / "b.bc"};
}
void propagation(const std::filesystem::path& fixtures) {
    const auto safe = units(fixtures / "safe"), unsafe = units(fixtures / "unsafe");
    const auto resolved = resolveProgramInputs(safe);
    require(resolved == resolveProgramInputs({fixtures / "safe"}), "directory and explicit files differ");
    auto reversed = safe; std::reverse(reversed.begin(), reversed.end());
    require(resolved == resolveProgramInputs(reversed), "input ordering not deterministic");
    reject([&] { resolveProgramInputs({fixtures / "safe", safe[0]}); }, "[duplicate-input]");
    reject([&] { resolveProgramInputs({fixtures / "absent.ll"}); }, "[input-missing]");
    reject([&] { resolveProgramInputs({fixtures / "empty"}); }, "[input-empty]");
    reject([] { resolveProgramInputs({}); }, "[input-empty]");
    const auto imported = readParameterGraph(safe, query("CrossB", 1, 1));
    const auto slice = buildSlice(imported.graph, imported.query);
    bool upstreamA = false, downstreamC = false, sourceA = false, sourceB = false, sourceC = false;
    for (const auto& n : slice.upstream.nodes) upstreamA |= imported.graph.node(n.node).function == "CrossA";
    for (const auto& n : slice.downstream.nodes) downstreamC |= imported.graph.node(n.node).function == "CrossC";
    for (NodeId id = 0; id < imported.graph.size(); ++id) {
        const auto& n = imported.graph.node(id);
        const auto filename = std::filesystem::path(n.source.file).filename().string();
        sourceA |= n.function == "CrossA" && filename == "a.cpp";
        sourceB |= n.function == "CrossB" && filename == "b.cpp";
        sourceC |= n.function == "CrossC" && filename == "c.cpp";
    }
    require(upstreamA && downstreamC && sourceA && sourceB && sourceC, "cross-file edges or source paths missing");
    const auto zero = readParameterGraph(safe, query("CrossB"));
    const auto zeroSlice = buildSlice(zero.graph, zero.query);
    require(!zeroSlice.upstream.boundaries.empty() && !zeroSlice.downstream.boundaries.empty(), "linked query ignored depth=0");
    for (const auto variant : {"safe", "unsafe"}) {
        const auto directory = fixtures / variant;
        const auto q = query("CrossB", 1, 1);
        const auto multiple = verifyArithmeticRisks(units(directory), q);
        const auto single = verifyArithmeticRisks(fixtures / (std::string("reference_") + variant + ".ll"), q);
        require(multiple.candidates == single.candidates, "single/multifile candidate counts differ");
        require(!hasRisk(multiple, ArithmeticRiskReason::SignedOverflow) &&
                !hasRisk(single, ArithmeticRiskReason::SignedOverflow),
                "unsigned source type gained an inapplicable signed-overflow warning");
        require(risk(multiple, ArithmeticRiskReason::UnsignedWrap).status ==
                risk(single, ArithmeticRiskReason::UnsignedWrap).status, "single/multifile verification differs");
        const auto expected = std::string(variant) == "safe" ? ReportStatus::ScopedUnreachable : ReportStatus::BoundaryTriggerable;
        require(risk(multiple, ArithmeticRiskReason::UnsignedWrap).status == expected, "cross-file guard lost");
        if (expected == ReportStatus::BoundaryTriggerable) {
            const auto& e = *risk(multiple, ArithmeticRiskReason::UnsignedWrap).verification;
            const auto input = static_cast<unsigned>(std::stoull(e.inputs.at(0).unsignedValue));
            require(CrossA(input) == std::stoull(e.resultBitPattern) && observedCrossLength == input + 8u, "multifile risk witness failed native replay");
        }
        ReachabilityQuery reach;
        reach.target = query("CrossC", 2); reach.target.parameterNames = {"length"};
        reach.comparison = "ugt"; reach.value = "4096";
        const auto solved = solveParameterReachability(units(directory), reach);
        require(solved.status == (expected == ReportStatus::ScopedUnreachable ? SolveStatus::Unsat : SolveStatus::Sat), "multifile solve lost caller paths");
        if (solved.status == SolveStatus::Sat) {
            const auto input = static_cast<unsigned>(std::stoull(solved.inputs.at(0).unsignedValue));
            require(CrossA(input) == std::stoull(solved.targetValue) && observedCrossLength > 4096, "multifile solve witness failed native replay");
        }
    }
    const auto boundary = verifyArithmeticRisks(safe, query("CrossB"));
    require(risk(boundary, ArithmeticRiskReason::UnsignedWrap).status == ReportStatus::BoundaryTriggerable, "used out-of-scope upstream guard");
    const auto missing = verifyArithmeticRisks(safe, query("CrossAfter"));
    require(risk(missing, ArithmeticRiskReason::UnsignedWrap).status == ReportStatus::Unknown, "missing external definition falsely verified");
    const auto local = inspectParameters(safe, "localStep");
    require(local.size() == 2 && local[0].symbol != local[1].symbol && local[0].irInput != local[1].irInput &&
            local[0].source.file != local[1].source.file, "same-name static functions not isolated/selectable");
    for (const auto& candidate : local) {
        auto selected = query(candidate.symbol, 1);
        const auto r = verifyArithmeticRisks(safe, selected);
        const auto& e = *risk(r, ArithmeticRiskReason::UnsignedWrap).verification;
        const bool a = std::filesystem::path(candidate.source.file).filename() == "a.cpp";
        require(e.operands.at(1).unsignedValue == (a ? "1" : "2"), "static function body crossed a translation-unit boundary");
        const auto second = inspectParameters(reversed, "localStep");
        require(second[0].symbol == local[0].symbol && second[1].symbol == local[1].symbol, "local selection unstable across reloads");
    }
    require(StaticA(0) == 1 && StaticB(0) == 2, "native static replay mismatch");
}
void conflicts(const std::filesystem::path& fixtures) {
    const auto locals = inspectParameters(std::vector<std::filesystem::path>{fixtures / "locals_a.ll", fixtures / "locals_b.ll"}, "unusedLocal");
    require(locals.size() == 2 && locals[0].symbol != locals[1].symbol && locals[0].irInput != locals[1].irInput,
            "linker discarded an unreferenced static analysis root or lost its provenance");
    for (const auto& [name, diagnostic] : std::vector<std::pair<std::string, std::string>>{
            {"type", "[symbol-type-mismatch]"}, {"duplicate", "[duplicate-definition]"},
            {"weak", "[duplicate-definition]"}, {"target", "[target-mismatch]"},
            {"layout", "[target-mismatch]"}, {"abi", "[function-abi-mismatch]"},
            {"flags", "[module-link]"}, {"globals", "[symbol-type-mismatch]"}}) {
        reject([&] { inspectParameters(std::vector<std::filesystem::path>{fixtures / (name + "_a.ll"), fixtures / (name + "_b.ll")}, "anything"); }, diagnostic);
    }
}
}
int main(int argc, char** argv) {
    try {
        require(argc == 3, "expected generated and hand-written fixture directories");
        propagation(argv[1]); conflicts(argv[2]);
        std::cout << "Cross-file propagation, verification, replay and conflict checks passed\n"; return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
