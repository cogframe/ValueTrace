#include "valuetrace/ArithmeticReport.h"

#include <algorithm>
#include <ostream>
#include <set>
#include <vector>

namespace valuetrace {
namespace {
const char* typeName(SourceIntegerKind kind) {
    switch (kind) {
    case SourceIntegerKind::Signed: return "signed";
    case SourceIntegerKind::Unsigned: return "unsigned";
    case SourceIntegerKind::Boolean: return "boolean";
    case SourceIntegerKind::Unknown: return "unknown";
    }
    return "unknown";
}
}

ReportSummary writeArithmeticReport(std::ostream& output, const DependencyGraph& graph,
                                   const SliceResult& slice, const ReportFilter& filter) {
    std::set<NodeId> included;
    for (const auto& reached : slice.upstream.nodes) included.insert(reached.node);
    for (const auto& reached : slice.downstream.nodes) included.insert(reached.node);
    std::vector<ReportStatus> statuses;
    for (auto id : included) {
        const auto& node = graph.node(id);
        for (const auto& risk : node.arithmeticRisks) {
            statuses.push_back(risk.status);
            if (!filter.includes(risk.status)) continue;
            output << "Warning: ArithmeticRisk reason=" << arithmeticRiskReasonName(risk.reason)
                   << " feasibility=" << reportStatusName(risk.status)
                   << " status=" << reportStatusName(risk.status);
            if (risk.assumption == IntegerInterpretation::Signed || risk.assumption == IntegerInterpretation::Unsigned) {
                const bool signedRisk = risk.assumption == IntegerInterpretation::Signed;
                const auto expected = signedRisk ? SourceIntegerKind::Signed : SourceIntegerKind::Unsigned;
                const auto name = signedRisk ? "signed" : "unsigned";
                if (node.integerType.conflict.empty() && node.integerType.sourceKind == expected)
                    output << " source-type=" << name;
                else if (node.integerType.conflict.empty() && node.integerType.sourceOrAttribute &&
                         node.integerType.kind == expected)
                    output << " attribute-type=" << name;
                else output << " assumption=" << name;
            }
            output << " node=#" << id << " function=" << (node.displayFunction.empty() ? node.function : node.displayFunction)
                   << " context=" << node.context;
            if (!node.source.file.empty()) output << " source=" << node.source.file << ':' << node.source.line;
            if (!node.operation.ir.empty()) output << " instruction=" << node.operation.ir;
            output << '\n';
            output << "  Type: source=" << typeName(node.integerType.sourceKind)
                   << " resolved=" << typeName(node.integerType.kind)
                   << " basis=" << node.integerType.basis << '\n';
            if (!node.integerType.conflict.empty()) output << "  Type conflict: " << node.integerType.conflict << '\n';
            if (risk.verification) {
                const auto& evidence = *risk.verification;
                if (risk.status == ReportStatus::ScopedUnreachable || risk.status == ReportStatus::BoundaryTriggerable) {
                    output << "  Used assumptions: free-values=";
                    if (evidence.freeValues.empty()) output << "none";
                    else for (std::size_t i = 0; i < evidence.freeValues.size(); ++i)
                        output << (i ? "; " : "") << evidence.freeValues[i];
                    output << "; upstream-no-wrap-overflow="
                           << (evidence.noWrapSemantics == "wrap" ? "wrap" : "enforced")
                           << "; other-UB-or-poison=enforced; debug-type-narrowing="
                           << (node.integerType.sourceKind != SourceIntegerKind::Unknown && node.integerType.conflict.empty()
                               ? "risk-kind-only" : "no") << "; debug-bitvector-domain-narrowing=no\n";
                    const bool usesRand = std::any_of(evidence.freeValues.begin(), evidence.freeValues.end(),
                        [](const std::string& value) { return value.starts_with("rand.call"); });
                    output << "  Rand range: " << (usesRand ? evidence.randRangeBasis : "not used") << '\n';
                }
                output << "  Unmodeled dependencies: count=" << evidence.unmodeledDependencies.size() << '\n';
                for (const auto& dependency : evidence.unmodeledDependencies)
                    output << "  Unmodeled dependency: " << dependency << '\n';
                output << "  Over-approximation dependencies: count=" << evidence.overapproximatedDependencies.size() << '\n';
                for (const auto& dependency : evidence.overapproximatedDependencies)
                    output << "  Over-approximation dependency: " << dependency << '\n';
                output << "  Discarded dependencies: count=" << evidence.discardedDependencies.size() << '\n';
                for (const auto& dependency : evidence.discardedDependencies)
                    output << "  Discarded dependency: " << dependency << '\n';
                output << "  Unexplored paths: count=" << evidence.unexploredPaths.size() << '\n';
                for (const auto& path : evidence.unexploredPaths)
                    output << "  Unexplored path: " << path << '\n';
                output << "  Verification: solver=" << evidence.solverResult << " source-closed="
                       << (evidence.sourceClosed ? "true" : "false") << " detail=" << evidence.detail << '\n';
                output << "  Semantics: " << evidence.noWrapSemantics << '\n';
                const auto printValue = [&](const char* kind, const WitnessValue& value) {
                    output << "  " << kind << ' ' << value.name << " i" << value.bitWidth
                           << " unsigned=" << value.unsignedValue << " signed=" << value.signedValue << '\n';
                };
                for (const auto& value : evidence.inputs) printValue("Boundary input", value);
                for (const auto& value : evidence.randCalls) printValue("Rand call", value);
                for (const auto& value : evidence.operands) printValue("Operand", value);
                output << "  Memory evidence: count=" << evidence.memoryEvidence.size() << '\n';
                for (const auto& item : evidence.memoryEvidence) output << "  Memory evidence: " << item << '\n';
                if (evidence.noDefinedResult) output << "  Effect: undefined behavior at division or remainder\n";
                else if (evidence.resultIsPoison && risk.reason == ArithmeticRiskReason::InvalidShiftAmount)
                    output << "  Effect: poison (shift amount is at least the bit width)\n";
                else if (evidence.resultIsPoison) {
                    output << "  Effect: poison (violated target flag";
                    if (node.operation.noSignedWrap) output << " nsw";
                    if (node.operation.noUnsignedWrap) output << " nuw";
                    output << ")\n";
                } else if (!evidence.resultBitPattern.empty() &&
                           (node.operation.opcode == "add" || node.operation.opcode == "sub" ||
                            node.operation.opcode == "mul" || node.operation.opcode == "shl")) {
                    if (evidence.noWrapSemantics == "wrap" &&
                        (node.operation.noSignedWrap || node.operation.noUnsignedWrap))
                        output << "  Effect: hypothetical wrap mode ignores target nsw/nuw; modulo-2^"
                               << node.bitWidth << " bit pattern\n";
                    else if (risk.reason == ArithmeticRiskReason::SignedOverflow &&
                        node.integerType.sourceKind == SourceIntegerKind::Signed && node.integerType.conflict.empty())
                        output << "  Effect: LLVM IR modulo-2^" << node.bitWidth
                               << " bit pattern (no target flag violated); source signed overflow remains a candidate\n";
                    else output << "  Effect: defined modulo-2^" << node.bitWidth
                                << " bit pattern (no target flag violated)\n";
                }
                else if (!evidence.resultBitPattern.empty() && node.operation.opcode == "trunc")
                    output << "  Effect: defined low-bit truncation (no target flag violated)\n";
                if (evidence.noDefinedResult) output << "  Result: no defined LLVM result for the failing operation\n";
                else if (!evidence.resultBitPattern.empty()) output << "  Result: modular-bit-pattern=" << evidence.resultBitPattern
                    << " i" << node.bitWidth
                    << (evidence.resultIsPoison ? " ir-value=poison (bit pattern is not a valid IR result)" : " ir-value=defined") << '\n';
                else if (evidence.resultIsPoison) output << "  Result: ir-value=poison (no meaningful result bit pattern)\n";
                for (const auto& step : evidence.path) output << "  Path: " << step << '\n';
                for (const auto& condition : evidence.constraints) output << "  Constraint: " << condition << '\n';
                for (const auto& note : evidence.notes) {
                    if (note.starts_with("Potential undefined behavior:"))
                        output << "  Undefined behavior candidate:" << note.substr(sizeof("Potential undefined behavior:") - 1) << '\n';
                    else output << "  Model note: " << note << '\n';
                }
            } else output << "  Unmodeled dependencies: not checked\n";
        }
    }
    const auto summary = summarizeReports(statuses, filter);
    output << "Arithmetic report summary: total=" << summary.totalCount() << " shown=" << summary.shownCount()
           << " hidden=" << summary.hiddenCount() << " count-unit=risk-reasons filter=" << filter.description() << '\n';
    for (std::size_t i = 0; i < reportStatuses.size(); ++i) {
        output << "  status=" << reportStatusName(reportStatuses[i]) << " total=" << summary.totals[i]
               << " shown=" << summary.shown[i] << " hidden=" << summary.totals[i] - summary.shown[i] << '\n';
    }
    output << "Display filtering does not change analysis conclusions; zero displayed warnings does not prove safety.\n";
    return summary;
}

} // namespace valuetrace
