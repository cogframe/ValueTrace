#include "valuetrace/RiskVerifier.h"
#include "valuetrace/ArithmeticReport.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <algorithm>

using namespace valuetrace;
extern "C" unsigned RiskA(unsigned);
extern volatile unsigned observedRiskLength;
namespace {
void require(bool ok, const std::string& message) {
    if (!ok) throw std::runtime_error(message);
}
ParameterQuery query(std::string function, unsigned up = 0, unsigned down = 0) {
    ParameterQuery q;
    q.function = std::move(function);
    q.parameters = {0}; q.upDepth = up; q.downDepth = down;
    return q;
}
std::vector<Node> nodes(const RiskVerificationReport& report) {
    std::vector<Node> result;
    for (NodeId id = 0; id < report.program.graph.size(); ++id) result.push_back(report.program.graph.node(id));
    return result;
}
const ArithmeticRisk& find(const RiskVerificationReport& report, ArithmeticRiskReason reason,
                           std::string instruction = {}) {
    for (NodeId id = 0; id < report.program.graph.size(); ++id) {
        const auto& node = report.program.graph.node(id);
        if (!instruction.empty() && node.label != instruction) continue;
        for (const auto& risk : node.arithmeticRisks) {
            if (risk.reason == reason && risk.verification) return risk;
        }
    }
    throw std::runtime_error("verified risk not found: " + instruction);
}
void expect(const RiskVerificationReport& r, ArithmeticRiskReason reason, ReportStatus status,
            std::string instruction = {}) {
    const auto& risk = find(r, reason, instruction);
    require(risk.status == status, std::string("unexpected status for ") + arithmeticRiskReasonName(reason) + " " + instruction + ": " + risk.verification->detail);
    require(!risk.verification->sourceClosed, "invented source closure");
}
void irCases(const std::filesystem::path& file) {
    expect(verifyArithmeticRisks(file, query("intrinsic_abs")),
           ArithmeticRiskReason::SignedOverflow, ReportStatus::ScopedUnreachable, "%sum");
    expect(verifyArithmeticRisks(file, query("intrinsic_minmax")),
           ArithmeticRiskReason::SignedOverflow, ReportStatus::ScopedUnreachable, "%sum");
    expect(verifyArithmeticRisks(file, query("intrinsic_uminmax")),
           ArithmeticRiskReason::UnsignedWrap, ReportStatus::ScopedUnreachable, "%sum");
    ParameterQuery returnQuery;
    returnQuery.function = "return_source";
    returnQuery.selectReturn = true;
    returnQuery.upDepth = 0;
    returnQuery.downDepth = 2;
    expect(verifyArithmeticRisks(file, returnQuery),
           ArithmeticRiskReason::SignedOverflow, ReportStatus::BoundaryTriggerable, "%sum");
    ParameterQuery randQuery;
    randQuery.function = "rand_small";
    randQuery.selectReturn = true;
    randQuery.upDepth = 0;
    randQuery.downDepth = 0;
    VerificationOptions bounded;
    bounded.randMaxOverride = 32767;
    const auto boundedRand = verifyArithmeticRisks(file, randQuery, bounded);
    expect(boundedRand, ArithmeticRiskReason::SignedOverflow, ReportStatus::ScopedUnreachable, "%sum");
    VerificationOptions unbounded;
    unbounded.randUnbounded = true;
    const auto unboundedRand = verifyArithmeticRisks(file, randQuery, unbounded);
    expect(unboundedRand, ArithmeticRiskReason::SignedOverflow, ReportStatus::BoundaryTriggerable, "%sum");
    require(!find(unboundedRand, ArithmeticRiskReason::SignedOverflow, "%sum").verification->randCalls.empty(),
            "rand witness did not record the contributing call");
    for (const auto* name : {"raw", "raw64", "late_guard", "multiply"}) {
        const auto r = verifyArithmeticRisks(file, query(name));
        for (const auto reason : {ArithmeticRiskReason::SignedOverflow, ArithmeticRiskReason::UnsignedWrap}) {
            expect(r, reason, ReportStatus::BoundaryTriggerable);
            const auto& e = *find(r, reason).verification;
            require(e.resultIsPoison && !e.inputs.empty() && e.operands.size() == 2 &&
                    !e.path.empty() && !e.constraints.empty(), "own flags hid risk or witness missing");
            require(e.operands.front().bitWidth == (std::string(name) == "raw64" ? 64u : 32u), "lost bit width");
        }
    }
    for (const auto reason : {ArithmeticRiskReason::SignedOverflow, ArithmeticRiskReason::UnsignedWrap}) {
        expect(verifyArithmeticRisks(file, query("guarded")), reason, ReportStatus::ScopedUnreachable);
        // Same instruction, with and without its caller's guard in scope.
        const auto boundary = verifyArithmeticRisks(file, query("raw", 0));
        for (const auto& n : nodes(boundary)) for (const auto& risk : n.arithmeticRisks)
            if (risk.verification && risk.reason == reason) require(risk.status == ReportStatus::BoundaryTriggerable, "used caller beyond up=0");
        expect(verifyArithmeticRisks(file, query("guarded_parent", 0, 1)), reason, ReportStatus::ScopedUnreachable);
        const auto upstream = verifyArithmeticRisks(file, query("raw", 1));
        bool guardedCallerChecked = false;
        for (const auto& n : nodes(upstream)) for (const auto& risk : n.arithmeticRisks)
            if (risk.verification && risk.reason == reason && n.rootFunction == "guarded_parent") {
                require(risk.status == ReportStatus::ScopedUnreachable, "up=1 lost caller guard");
                guardedCallerChecked = true;
            }
        require(guardedCallerChecked, "guarded upstream call not checked");
    }
    expect(verifyArithmeticRisks(file, query("subtract")), ArithmeticRiskReason::UnsignedUnderflow, ReportStatus::ScopedUnreachable);
    const auto priorFlag = verifyArithmeticRisks(file, query("prior_flag"));
    expect(priorFlag, ArithmeticRiskReason::UnsignedUnderflow, ReportStatus::Unknown, "%sum");
    require(find(priorFlag, ArithmeticRiskReason::UnsignedUnderflow, "%sum").verification->detail.find(
                "仅在无 UB 假设下不可触发") != std::string::npos,
            "poison-dependent unreachability was not downgraded");
    const auto guardedWrapDefault = verifyArithmeticRisks(file, query("nsw_guard_wrap"));
    expect(guardedWrapDefault, ArithmeticRiskReason::SignedOverflow, ReportStatus::Unknown, "%sum");
    require(find(guardedWrapDefault, ArithmeticRiskReason::SignedOverflow, "%sum").verification->detail.find(
                "仅在无 UB 假设下不可触发") != std::string::npos,
            "ub-free-only exclusion did not carry its explicit tag");
    VerificationOptions wrapMode;
    wrapMode.noWrapSemantics = NoWrapSemantics::Wrap;
    const auto guardedWrap = verifyArithmeticRisks(file, query("nsw_guard_wrap"), wrapMode);
    expect(guardedWrap, ArithmeticRiskReason::SignedOverflow, ReportStatus::BoundaryTriggerable, "%sum");
    for (const auto* name : {"divide", "remainder"}) {
        const auto r = verifyArithmeticRisks(file, query(name));
        for (const auto reason : {ArithmeticRiskReason::DivideByZero, ArithmeticRiskReason::SignedDivisionOverflow}) {
            expect(r, reason, ReportStatus::BoundaryTriggerable);
            require(find(r, reason).verification->noDefinedResult, "invented division failure result");
        }
    }
    for (const auto reason : {ArithmeticRiskReason::DivideByZero, ArithmeticRiskReason::SignedDivisionOverflow})
        expect(verifyArithmeticRisks(file, query("safe_divide")), reason, ReportStatus::ScopedUnreachable);
    const auto contains = [](const std::vector<std::string>& entries, const std::string& text) {
        return std::any_of(entries.begin(), entries.end(), [&](const auto& entry) {
            return entry.find(text) != std::string::npos;
        });
    };
    for (const auto* name : {"memory", "guarded_memory", "unknown_call", "indirect_value", "unsupported_value"}) {
        const auto unresolved = verifyArithmeticRisks(file, query(name));
        expect(unresolved, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown);
        const auto& dependencies = find(unresolved, ArithmeticRiskReason::UnsignedWrap).verification->unmodeledDependencies;
        require(!dependencies.empty(), "unknown risk lost its unmodeled dependency list");
        const auto expected = std::string(name) == "unknown_call" ? "external call" :
            std::string(name) == "indirect_value" ? "indirect call" :
            (std::string(name) == "memory" || std::string(name) == "guarded_memory") ?
                "unmodeled entry memory value" : "unsupported value dependency";
        require(std::any_of(dependencies.begin(), dependencies.end(), [&](const auto& item) {
                    return item.find(expected) != std::string::npos;
                }),
                "unmodeled dependency did not identify the missing value source");
        if (std::string(name) == "memory")
            require(find(unresolved, ArithmeticRiskReason::UnsignedWrap).verification->solverResult == "UNKNOWN",
                    "arbitrary load value was reported as a concrete SAT witness");
        if (std::string(name) == "indirect_value")
            require(std::any_of(dependencies.begin(), dependencies.end(), [](const auto& item) {
                        return item.find("indirect call result") != std::string::npos;
                    }), "indirect call result was silently dropped before the risk operation");
    }
    const auto arbitrarySafe = verifyArithmeticRisks(file, query("rand_guarded"));
    expect(arbitrarySafe, ArithmeticRiskReason::UnsignedWrap, ReportStatus::ScopedUnreachable, "%sum");
    const auto& arbitraryEvidence = *find(arbitrarySafe, ArithmeticRiskReason::UnsignedWrap, "%sum").verification;
    require(contains(arbitraryEvidence.overapproximatedDependencies, "rand") &&
            arbitraryEvidence.discardedDependencies.empty() &&
            arbitraryEvidence.detail.find("任意值成立") != std::string::npos,
            "UNSAT with arbitrary rand was not preserved as scoped-unreachable");
    std::ostringstream arbitraryReport;
    writeArithmeticReport(arbitraryReport, arbitrarySafe.program.graph, arbitrarySafe.slice, ReportFilter::forLevel(5));
    require(arbitraryReport.str().find("Over-approximation dependency:") != std::string::npos &&
            arbitraryReport.str().find("任意值成立") != std::string::npos,
            "rendered report omitted the arbitrary-value proof or dependency class");
    const auto declared = verifyArithmeticRisks(file, query("rand_risk"));
    expect(declared, ArithmeticRiskReason::UnsignedWrap, ReportStatus::BoundaryTriggerable, "%sum");
    const auto& declaredEvidence = *find(declared, ArithmeticRiskReason::UnsignedWrap, "%sum").verification;
    require(contains(declaredEvidence.overapproximatedDependencies, "rand") &&
            declaredEvidence.detail.find("范围未验证") != std::string::npos &&
            !declaredEvidence.operands.empty() && !declaredEvidence.randCalls.empty(),
            "declared rand SAT lost source note or witness");
    auto configured = VerificationOptions{};
    configured.externalInputSources.push_back("external");
    expect(verifyArithmeticRisks(file, query("unknown_call"), configured),
           ArithmeticRiskReason::UnsignedWrap, ReportStatus::BoundaryTriggerable, "%sum");
    const auto closed = verifyArithmeticRisks(file, query("closed_sink", 3));
    expect(closed, ArithmeticRiskReason::UnsignedWrap, ReportStatus::ScopedUnreachable, "%sum");
    const auto& closedEvidence = *find(closed, ArithmeticRiskReason::UnsignedWrap, "%sum").verification;
    require(closedEvidence.unexploredPaths.empty() &&
            contains(closedEvidence.notes, "Data slice closed at upstream depth 1"),
            "no-argument source did not close the call slice at its data origin");
    const auto constantClosed = verifyArithmeticRisks(file, query("constant_sink", 3));
    expect(constantClosed, ArithmeticRiskReason::UnsignedWrap, ReportStatus::ScopedUnreachable, "%sum");
    const auto& constantEvidence = *find(constantClosed, ArithmeticRiskReason::UnsignedWrap, "%sum").verification;
    require(constantEvidence.unexploredPaths.empty() &&
            contains(constantEvidence.notes, "Data slice closed at upstream depth 1: all call arguments are integer constants"),
            "constant call did not record the closed depth when its caller has an unrelated parameter");
    const auto entryLoadReport = verifyArithmeticRisks(file, query("memory"));
    const auto& entryLoad = *find(entryLoadReport, ArithmeticRiskReason::UnsignedWrap).verification;
    require(!entryLoad.overapproximatedDependencies.empty() && entryLoad.discardedDependencies.empty(),
            "unselected entry load was not classified as an arbitrary value");
    require(find(verifyArithmeticRisks(file, query("guarded")), ArithmeticRiskReason::UnsignedWrap)
                .verification->unmodeledDependencies.empty(), "fully modeled guard gained a false dependency");
    const auto independent = verifyArithmeticRisks(file, query("independent_memory"));
    expect(independent, ArithmeticRiskReason::UnsignedWrap, ReportStatus::ScopedUnreachable, "%sum");
    require(find(independent, ArithmeticRiskReason::UnsignedWrap, "%sum")
                .verification->unmodeledDependencies.empty(), "unrelated load/store contaminated the risk slice");
    const auto controlOnly = verifyArithmeticRisks(file, query("control_only_memory"));
    expect(controlOnly, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown, "%sum");
    const auto& controlEvidence = *find(controlOnly, ArithmeticRiskReason::UnsignedWrap, "%sum").verification;
    require(contains(controlEvidence.unmodeledDependencies, "unsupported control dependency: unmodeled entry memory value") &&
            controlEvidence.unexploredPaths.empty(),
            "unmodeled control-only load was lost from the path condition");
    const auto poisoned = verifyArithmeticRisks(file, query("poison_before_mul"));
    expect(poisoned, ArithmeticRiskReason::SignedOverflow, ReportStatus::BoundaryTriggerable, "%first");
    require(find(poisoned, ArithmeticRiskReason::SignedOverflow, "%first").verification->resultIsPoison,
            "first poison producer was not identified");
    expect(poisoned, ArithmeticRiskReason::SignedOverflow, ReportStatus::Unknown, "%later");
    expect(poisoned, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown, "%later");
    const auto poisonedDivisor = verifyArithmeticRisks(file, query("poison_divisor_risk"));
    expect(poisonedDivisor, ArithmeticRiskReason::SignedOverflow, ReportStatus::BoundaryTriggerable, "%first");
    expect(poisonedDivisor, ArithmeticRiskReason::DivideByZero, ReportStatus::ScopedUnreachable, "%later");
    require(contains(find(poisonedDivisor, ArithmeticRiskReason::DivideByZero, "%later")
                .verification->notes, "Potential undefined behavior: poison used as division or remainder divisor"),
            "risk report did not classify poison divisor use as undefined behavior");
    std::ostringstream poisonReport;
    writeArithmeticReport(poisonReport, poisonedDivisor.program.graph, poisonedDivisor.slice, ReportFilter::forLevel(5));
    require(poisonReport.str().find("Undefined behavior candidate: poison used as division or remainder divisor") != std::string::npos,
            "rendered report did not classify poison divisor use as undefined behavior");
    const auto mixed = verifyArithmeticRisks(file, query("mixed_modeled_unknown"));
    expect(mixed, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown, "%sum");
    const auto& mixedEvidence = *find(mixed, ArithmeticRiskReason::UnsignedWrap, "%sum").verification;
    require(contains(mixedEvidence.unmodeledDependencies, "unmodeled entry memory value") && mixedEvidence.inputs.empty(),
            "SAT from one branch leaked a counterexample despite another unmodeled branch");
    const auto boundaryReturn = verifyArithmeticRisks(file, query("outside_return", 0, 0));
    expect(boundaryReturn, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown, "%sum");
    require(contains(find(boundaryReturn, ArithmeticRiskReason::UnsignedWrap, "%sum")
                .verification->unmodeledDependencies, "boundary_return"),
            "out-of-scope return value was omitted from the dependency list");
    expect(verifyArithmeticRisks(file, query("outside_return", 0, 1)),
           ArithmeticRiskReason::UnsignedWrap, ReportStatus::ScopedUnreachable, "%sum");
    const auto escaped = verifyArithmeticRisks(file, query("outside_pointer", 0, 0));
    expect(escaped, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown, "%sum");
    const auto& escapedDependencies = find(escaped, ArithmeticRiskReason::UnsignedWrap, "%sum")
                                          .verification->unmodeledDependencies;
    require(contains(escapedDependencies, "地址已逃逸（待 6b）") &&
            contains(find(escaped, ArithmeticRiskReason::UnsignedWrap, "%sum").verification->memoryEvidence,
                     "boundary_write"),
            "escaped alloca read or out-of-scope pointer write missing from memory evidence");
    for (const auto* name : {"escape_select", "escape_gep", "escape_phi"}) {
        const auto forwarded = verifyArithmeticRisks(file, query(name));
        expect(forwarded, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown, "%sum");
        require(contains(find(forwarded, ArithmeticRiskReason::UnsignedWrap, "%sum")
                         .verification->unmodeledDependencies, "地址已逃逸（待 6b）"),
                "forwarded pointer address escaped without downgrading SAT");
    }
    const auto loop = verifyArithmeticRisks(file, query("loop_cut"));
    expect(loop, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown, "%sum");
    const auto& loopDependencies = find(loop, ArithmeticRiskReason::UnsignedWrap, "%sum").verification->unmodeledDependencies;
    require(std::any_of(loopDependencies.begin(), loopDependencies.end(), [](const auto& item) {
                return item.find("loop back-edge") != std::string::npos;
            }), "loop truncation was omitted from the dependency list");
    require(contains(find(loop, ArithmeticRiskReason::UnsignedWrap, "%sum")
                .verification->unexploredPaths, "loop back-edge"),
            "loop back-edge was treated as an arbitrary value instead of an unexplored path");
    VerificationOptions tiny;
    tiny.maxSteps = 1;
    auto exhausted = verifyArithmeticRisks(file, query("guarded"), tiny);
    require(exhausted.candidates == 2, "budget dropped candidates");
    expect(exhausted, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown);
    require(!find(exhausted, ArithmeticRiskReason::UnsignedWrap).verification->unmodeledDependencies.empty(),
            "budget exhaustion was omitted from the dependency list");
    require(contains(find(exhausted, ArithmeticRiskReason::UnsignedWrap).verification->unexploredPaths,
                     "instruction step budget reached"),
            "budget exhaustion was not recorded as an unexplored path");
    VerificationOptions tinyStates;
    tinyStates.maxStates = 1;
    const auto stateExhausted = verifyArithmeticRisks(file, query("guarded"), tinyStates);
    expect(stateExhausted, ArithmeticRiskReason::UnsignedWrap, ReportStatus::Unknown);
    require(contains(find(stateExhausted, ArithmeticRiskReason::UnsignedWrap).verification->unexploredPaths,
                     "path state budget reached"), "state budget exhaustion was not recorded as an unexplored path");
    const auto calls = verifyArithmeticRisks(file, query("two_calls", 0, 1));
    unsigned sat = 0, unsat = 0;
    for (const auto& n : nodes(calls)) for (const auto& risk : n.arithmeticRisks) {
        if (!risk.verification || risk.reason != ArithmeticRiskReason::UnsignedWrap) continue;
        sat += risk.status == ReportStatus::BoundaryTriggerable;
        unsat += risk.status == ReportStatus::ScopedUnreachable;
    }
    require(sat == 1 && unsat == 1, "different guarded call contexts merged");
    auto group = query("multiply"); group.parameters.push_back(1);
    require(verifyArithmeticRisks(file, group).candidates == 2, "parameter group duplicates or loses candidates");
}
void cppCases(const std::filesystem::path& safe, const std::filesystem::path& unsafe,
              const std::filesystem::path& bc) {
    auto q = query("RiskB", 1, 1);
    q.parameters.clear(); q.parameterNames = {"count"};
    expect(verifyArithmeticRisks(safe, q), ArithmeticRiskReason::UnsignedWrap, ReportStatus::ScopedUnreachable);
    expect(verifyArithmeticRisks(bc, q), ArithmeticRiskReason::UnsignedWrap, ReportStatus::ScopedUnreachable);
    const auto r = verifyArithmeticRisks(unsafe, q);
    expect(r, ArithmeticRiskReason::UnsignedWrap, ReportStatus::BoundaryTriggerable);
    const auto& e = *find(r, ArithmeticRiskReason::UnsignedWrap).verification;
    const auto x = std::stoull(e.operands.at(0).unsignedValue);
    require(x + 8 > UINT32_MAX && static_cast<std::uint32_t>(x + 8) == std::stoull(e.resultBitPattern), "wrap witness does not replay");
    require(e.inputs.size() == 1, "unexpected real C++ boundary inputs");
    const auto input = static_cast<unsigned>(std::stoull(e.inputs.front().unsignedValue));
    require(RiskA(input) == std::stoull(e.resultBitPattern) && observedRiskLength == input + 8u,
            "witness did not replay through actual A/B/C functions");
    q.upDepth = 0;
    expect(verifyArithmeticRisks(safe, q), ArithmeticRiskReason::UnsignedWrap, ReportStatus::BoundaryTriggerable);
    q = query("RiskA", 0, 0);
    require(verifyArithmeticRisks(unsafe, q).candidates == 0, "checked outside down=0");
    q.downDepth = 1;
    expect(verifyArithmeticRisks(unsafe, q), ArithmeticRiskReason::UnsignedWrap, ReportStatus::BoundaryTriggerable);
}
}
int main(int argc, char** argv) {
    try {
        require(argc == 5, "expected IR and three C++ fixtures");
        irCases(argv[1]); cppCases(argv[2], argv[3], argv[4]);
        std::cout << "Automatic risk verification passed\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
