#include "valuetrace/ArithmeticReport.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace valuetrace;

namespace {

void require(bool condition, const std::string& reason) {
    if (!condition) throw std::runtime_error(reason);
}

template<class Action>
void mustReject(Action action) {
    bool rejected = false;
    try { action(); } catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "invalid report filter accepted");
}

void checkPresets() {
    for (unsigned level = 1; level <= 5; ++level) {
        const auto filter = ReportFilter::forLevel(level);
        for (std::size_t i = 0; i < reportStatuses.size(); ++i) {
            require(filter.includes(reportStatuses[i]) == (i < level), "preset includes wrong verification state");
        }
        const auto counts = summarizeReports(reportStatuses, filter);
        require(counts.totalCount() == 5 && counts.shownCount() == level && counts.hiddenCount() == 5 - level,
                "preset count/hidden totals wrong");
    }
    const ReportFilter defaults;
    require(defaults.includes(ReportStatus::BoundaryTriggerable) && !defaults.includes(ReportStatus::Unknown) &&
            !defaults.includes(ReportStatus::Unverified), "default must be display level 2");
    mustReject([] { ReportFilter::forLevel(0); });
    mustReject([] { ReportFilter::forLevel(6); });
}

void checkExplicitStatuses() {
    const auto filter = ReportFilter::forStatuses(" unverified, unknown, unverified ");
    require(filter.includes(ReportStatus::Unverified) && filter.includes(ReportStatus::Unknown) &&
            !filter.includes(ReportStatus::SourceConfirmed), "explicit filter behaved as severity threshold");
    const auto none = summarizeReports(reportStatuses, ReportFilter::forStatuses("none"));
    require(none.shownCount() == 0 && none.hiddenCount() == 5, "none filter lost hidden reports");
    const auto all = summarizeReports(reportStatuses, ReportFilter::forStatuses("all"));
    require(all.shownCount() == 5 && all.hiddenCount() == 0, "all filter lost reports");
    for (const auto* invalid : {"", "bogus", "Unverified", "unknown,", ",unknown", "unknown,,unverified", "all,unknown", "none,unknown"}) {
        mustReject([&] { ReportFilter::forStatuses(invalid); });
    }
}

void checkReportDoesNotVerify() {
    DependencyGraph graph;
    Node node{"B", "%sum", 32, NodeKind::Operation, "call1"};
    node.arithmeticRisks = {{ArithmeticRiskReason::SignedOverflow, IntegerInterpretation::Signed},
                            {ArithmeticRiskReason::UnsignedWrap, IntegerInterpretation::Unsigned}};
    const auto id = graph.addNode(node);
    const SliceResult slice{{{{id, 0}}, {}}, {{{id, 0}}, {}}};
    std::ostringstream hiddenText;
    const auto hidden = writeArithmeticReport(hiddenText, graph, slice);
    require(hidden.totalCount() == 2 && hidden.shownCount() == 0 && hidden.hiddenCount() == 2,
            "default filter discarded candidates or double counted upstream/downstream");
    require(hiddenText.str().find("Warning: ArithmeticRisk") == std::string::npos &&
            hiddenText.str().find("total=2 shown=0 hidden=2") != std::string::npos,
            "hidden reports were printed or not counted");
    require(hiddenText.str().find("does not prove safety") != std::string::npos, "empty display could imply safety");
    std::ostringstream shownText;
    const auto shown = writeArithmeticReport(shownText, graph, slice, ReportFilter::forLevel(4));
    require(shown.totalCount() == 2 && shown.shownCount() == 2 && shown.hiddenCount() == 0, "level 4 did not show candidates");
    require(shownText.str().find("reason=SignedOverflow feasibility=unverified") != std::string::npos &&
            shownText.str().find("reason=UnsignedWrap feasibility=unverified") != std::string::npos, "original reasons or status lost");
    for (const auto& risk : graph.node(id).arithmeticRisks) require(risk.status == ReportStatus::Unverified, "display filtering upgraded an unverified risk");
    RiskVerification sat;
    sat.solverResult = "SAT";
    mustReject([&] { graph.setRiskVerification(id, 0, ReportStatus::SourceConfirmed, sat); });
    mustReject([&] { graph.setRiskVerification(id, 0, ReportStatus::ScopedUnreachable, sat); });
    RiskVerification incomplete;
    incomplete.solverResult = "UNSAT";
    incomplete.unmodeledDependencies = {"memory read dependency not modeled"};
    incomplete.discardedDependencies = {"unexplored path: loop backedge"};
    mustReject([&] { graph.setRiskVerification(id, 0, ReportStatus::ScopedUnreachable, incomplete); });
    incomplete.discardedDependencies.clear();
    incomplete.overapproximatedDependencies = incomplete.unmodeledDependencies;
    graph.setRiskVerification(id, 0, ReportStatus::ScopedUnreachable, incomplete);
    graph.setRiskVerification(id, 0, ReportStatus::BoundaryTriggerable, sat);
    require(graph.node(id).arithmeticRisks[0].verification.has_value(), "lost verification evidence");
    // Future verification producers can attach these states. This fixture is
    // synthetic and does not claim the current analyzer proves them.
    for (auto status : reportStatuses) {
        Node fixture{"fixture", "synthetic", 64, NodeKind::Operation, "root"};
        fixture.arithmeticRisks = {{ArithmeticRiskReason::SignedOverflow, IntegerInterpretation::Signed, status}};
        const auto nodeId = graph.addNode(fixture);
        std::ostringstream text;
        const SliceResult one{{{{nodeId, 0}}, {}}, {}};
        const auto report = writeArithmeticReport(text, graph, one, ReportFilter::forStatuses(reportStatusName(status)));
        require(report.shownCount() == 1 && report.totalCount() == 1, "reserved state cannot be represented or filtered");
    }
}

} // namespace

int main() {
    try {
        checkPresets();
        checkExplicitStatuses();
        checkReportDoesNotVerify();
        std::cout << "Five-state reporting, filters, counts and immutable candidate states passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
