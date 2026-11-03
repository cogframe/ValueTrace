#include "valuetrace/DependencyGraph.h"

#include <stdexcept>
#include <utility>

namespace valuetrace {

const char* arithmeticRiskReasonName(ArithmeticRiskReason reason) noexcept {
    switch (reason) {
    case ArithmeticRiskReason::SignedOverflow: return "SignedOverflow";
    case ArithmeticRiskReason::UnsignedWrap: return "UnsignedWrap";
    case ArithmeticRiskReason::UnsignedUnderflow: return "UnsignedUnderflow";
    case ArithmeticRiskReason::DivideByZero: return "DivideByZero";
    case ArithmeticRiskReason::SignedDivisionOverflow: return "SignedDivisionOverflow";
    case ArithmeticRiskReason::DownstreamConstraintViolation: return "DownstreamConstraintViolation";
    case ArithmeticRiskReason::SignedTruncationLoss: return "SignedTruncationLoss";
    case ArithmeticRiskReason::UnsignedTruncationLoss: return "UnsignedTruncationLoss";
    case ArithmeticRiskReason::InvalidShiftAmount: return "InvalidShiftAmount";
    }
    return "UnknownArithmeticRisk";
}

NodeId DependencyGraph::addNode(Node node) {
    if (node.bitWidth == 0 && node.kind != NodeKind::Use) {
        throw std::invalid_argument("node bit width must be positive");
    }
    const auto id = nodes_.size();
    nodes_.push_back(std::move(node));
    incoming_.emplace_back();
    outgoing_.emplace_back();
    return id;
}

void DependencyGraph::addEdge(NodeId from, NodeId to, EdgeKind kind,
                              std::string callSite, unsigned operandIndex) {
    node(from);
    node(to);
    if (kind == EdgeKind::CallBoundary && callSite.empty()) {
        throw std::invalid_argument("call boundary requires a call site identity");
    }
    if (kind == EdgeKind::Local &&
        (nodes_[from].function != nodes_[to].function ||
         nodes_[from].context != nodes_[to].context)) {
        throw std::invalid_argument("a local edge cannot cross functions or call contexts");
    }
    const auto id = edges_.size();
    edges_.push_back({from, to, kind, std::move(callSite), operandIndex});
    outgoing_[from].push_back(id);
    incoming_[to].push_back(id);
}

const Node& DependencyGraph::node(NodeId id) const { return nodes_.at(id); }
const Edge& DependencyGraph::edge(std::size_t id) const { return edges_.at(id); }
const std::vector<std::size_t>& DependencyGraph::incoming(NodeId id) const {
    return incoming_.at(id);
}
const std::vector<std::size_t>& DependencyGraph::outgoing(NodeId id) const {
    return outgoing_.at(id);
}
std::size_t DependencyGraph::size() const noexcept { return nodes_.size(); }

void DependencyGraph::setRiskVerification(NodeId id, std::size_t index, ReportStatus status, RiskVerification verification) {
    const bool sat = verification.solverResult == "SAT";
    if ((status == ReportStatus::BoundaryTriggerable && !sat) ||
        (status == ReportStatus::SourceConfirmed && (!sat || !verification.sourceClosed)) ||
        (status == ReportStatus::ScopedUnreachable && verification.solverResult != "UNSAT") ||
        (status == ReportStatus::Unknown && verification.solverResult != "UNKNOWN") ||
        (!verification.discardedDependencies.empty() && status != ReportStatus::Unknown) ||
        status == ReportStatus::Unverified) {
        throw std::invalid_argument("risk verification state does not match its evidence");
    }
    auto& risk = nodes_.at(id).arithmeticRisks.at(index);
    risk.status = status;
    risk.verification = std::move(verification);
}

} // namespace valuetrace
