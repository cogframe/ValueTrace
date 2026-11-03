#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <optional>
#include "valuetrace/ReportFilter.h"

namespace valuetrace {

using NodeId = std::size_t;

enum class NodeKind { Parameter, Operation, ReturnValue, Use, Constant };
enum class EdgeKind { Local, CallBoundary };

enum class IntegerInterpretation { BitPattern, Signed, Unsigned };
enum class SourceIntegerKind { Unknown, Signed, Unsigned, Boolean };

struct IntegerTypeEvidence {
    SourceIntegerKind kind = SourceIntegerKind::Unknown;
    SourceIntegerKind sourceKind = SourceIntegerKind::Unknown;
    bool sourceOrAttribute = false;
    std::string sourceType;
    std::string basis = "type unknown";
    std::string conflict;
};

struct WitnessValue {
    std::string name;
    unsigned bitWidth;
    std::string unsignedValue;
    std::string signedValue;
};

struct RiskVerification {
    std::string solverResult;
    std::string detail;
    std::vector<WitnessValue> inputs;
    std::vector<WitnessValue> operands;
    std::vector<WitnessValue> randCalls;
    std::vector<std::string> freeValues;
    std::vector<std::string> memoryEvidence;
    std::string randRangeBasis;
    std::vector<std::string> path;
    std::vector<std::string> constraints;
    std::vector<std::string> notes;
    std::vector<std::string> unmodeledDependencies;
    std::vector<std::string> overapproximatedDependencies;
    std::vector<std::string> discardedDependencies;
    std::vector<std::string> unexploredPaths;
    std::string resultBitPattern;
    bool resultIsPoison = false;
    bool noDefinedResult = false;
    bool sourceClosed = false;
    std::string noWrapSemantics = "ub-free";
};

// Public warnings share one category; the reason stays distinct for callers
// that later add path constraints or source-level checks.
enum class ArithmeticRiskReason {
    SignedOverflow,
    UnsignedWrap,
    UnsignedUnderflow,
    DivideByZero,
    SignedDivisionOverflow,
    DownstreamConstraintViolation,
    SignedTruncationLoss,
    UnsignedTruncationLoss,
    InvalidShiftAmount,
};

struct ArithmeticRisk {
    ArithmeticRiskReason reason;
    // Interpretation under which the risk predicate exists. Node::integerType
    // separately records whether source debug info or ABI attributes support it.
    IntegerInterpretation assumption = IntegerInterpretation::BitPattern;
    // Only an operation-specific verification may upgrade this state, never a
    // generic target-value query or a display filter.
    ReportStatus status = ReportStatus::Unverified;
    std::optional<RiskVerification> verification;
};

const char* arithmeticRiskReasonName(ArithmeticRiskReason reason) noexcept;

struct OperationSemantics {
    std::string opcode;
    IntegerInterpretation interpretation = IntegerInterpretation::BitPattern;
    std::string predicate;
    bool noSignedWrap = false;
    bool noUnsignedWrap = false;
    bool exact = false;
    bool nonNegative = false;
    bool sameSign = false;
    bool disjoint = false;
    std::string ir;
};

struct SourceLocation {
    std::string file;
    unsigned line = 0;
    unsigned column = 0;
};

struct CallSiteIdentity {
    std::string caller;
    unsigned instructionOrdinal;
    bool operator==(const CallSiteIdentity&) const = default;
};

// An LLVM integer type describes width only. Operation semantics belong to
// individual operations, not to a global signedness field on this node.
struct Node {
    std::string function;
    std::string label;
    unsigned bitWidth;
    NodeKind kind;
    std::string context;
    OperationSemantics operation;
    IntegerTypeEvidence integerType;
    SourceLocation source;
    std::string unresolved;
    std::string displayFunction;
    std::vector<ArithmeticRisk> arithmeticRisks;
    std::optional<unsigned> instructionOrdinal;
    std::string rootFunction;
    std::vector<CallSiteIdentity> callPath;
};

struct Edge {
    NodeId from;
    NodeId to;
    EdgeKind kind;
    std::string callSite;
    unsigned operandIndex = 0;
};

// The producer must keep different call contexts in separate nodes. This
// container preserves those identities; it does not resolve calls or aliases.
class DependencyGraph {
public:
    NodeId addNode(Node node);
    void addEdge(NodeId from, NodeId to, EdgeKind kind,
                 std::string callSite = {}, unsigned operandIndex = 0);

    const Node& node(NodeId id) const;
    const Edge& edge(std::size_t id) const;
    const std::vector<std::size_t>& incoming(NodeId id) const;
    const std::vector<std::size_t>& outgoing(NodeId id) const;
    std::size_t size() const noexcept;
    void setRiskVerification(NodeId node, std::size_t risk, ReportStatus status, RiskVerification verification);

private:
    std::vector<Node> nodes_;
    std::vector<Edge> edges_;
    std::vector<std::vector<std::size_t>> incoming_;
    std::vector<std::vector<std::size_t>> outgoing_;
};

} // namespace valuetrace
