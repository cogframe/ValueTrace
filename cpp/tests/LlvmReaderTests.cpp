#include "valuetrace/LlvmReader.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <set>

using namespace valuetrace;

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

const Node& named(const ImportedProgram& program, const std::string& label) {
    for (NodeId id = 0; id < program.graph.size(); ++id) {
        if (program.graph.node(id).label == label) return program.graph.node(id);
    }
    throw std::runtime_error("node missing: " + label);
}

template<class Action>
void mustFail(Action action, const std::string& reason) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, reason);
}

void checkCppChain(const std::filesystem::path& ll, const std::filesystem::path& bc) {
    ParameterQuery query{"B", {0}, 1, 1};
    const auto text = readParameterGraph(ll, query);
    const auto binary = readParameterGraph(bc, query);
    require(text.graph.size() == binary.graph.size(), "ll/bc graph sizes differ");
    require(text.query.parameters.size() == 1, "B should have one calling context");
    const auto selected = text.query.parameters.front();
    const auto slice = buildSlice(text.graph, text.query);
    const auto a = readParameterGraph(ll, {"A", {0}, 0, 0});
    const auto c = readParameterGraph(ll, {"C", {0}, 0, 0});
    const auto aSymbol = a.graph.node(a.query.parameters.front()).function;
    const auto cSymbol = c.graph.node(c.query.parameters.front()).function;
    require(std::any_of(slice.upstream.nodes.begin(), slice.upstream.nodes.end(), [&](const auto& reached) {
        const auto& node = text.graph.node(reached.node);
        return node.function == aSymbol && node.kind == NodeKind::Parameter && reached.callDepth == 1;
    }), "B upstream must reach A's parameter at depth one");
    require(std::any_of(slice.downstream.nodes.begin(), slice.downstream.nodes.end(), [&](const auto& reached) {
        const auto& node = text.graph.node(reached.node);
        return node.function == cSymbol && node.kind == NodeKind::Parameter && reached.callDepth == 1;
    }), "B downstream must reach C's parameter at depth one");
    const auto zero = buildSlice(text.graph, {{selected}, 0, 0});
    require(!zero.upstream.boundaries.empty(), "up zero must report caller boundary");
    require(!zero.downstream.boundaries.empty(), "down zero must report callee boundary");
    bool sourceFound = false, returnEdge = false, wideNode = false;
    for (NodeId id = 0; id < text.graph.size(); ++id) {
        const auto& node = text.graph.node(id);
        sourceFound |= node.source.line != 0 && node.source.file.find("parameter_chain.cpp") != std::string::npos;
        wideNode |= node.bitWidth == 64;
        for (auto edgeId : text.graph.outgoing(id)) {
            const auto& edge = text.graph.edge(edgeId);
            returnEdge |= node.kind == NodeKind::ReturnValue && edge.kind == EdgeKind::CallBoundary;
        }
    }
    require(sourceFound && wideNode && returnEdge, "source locations, i64 or return edges missing");
    const auto group = readParameterGraph(ll, {"B", {0, 1, 0}, 1, 1});
    require(group.query.parameters.size() == 2, "parameter group must deduplicate");
    mustFail([&] { readParameterGraph(ll, {"B", {99}, 1, 1}); }, "invalid index accepted");
    mustFail([&] { readParameterGraph(ll, {"absent", {0}, 1, 1}); }, "missing function accepted");
    auto limited = query;
    limited.maxNodes = 1;
    mustFail([&] { readParameterGraph(ll, limited); }, "node budget ignored");
    const auto depth64 = readParameterGraph(ll, {"B", {0}, 64, 64});
    require(!depth64.query.parameters.empty(), "depth 64 must be accepted in both directions");
    mustFail([&] { readParameterGraph(ll, {"B", {0}, 65, 0}); }, "up depth 65 must be rejected");
    mustFail([&] { readParameterGraph(ll, {"B", {0}, 0, 65}); }, "down depth 65 must be rejected");
}

void checkSemantics(const std::filesystem::path& fixtures) {
    const auto program = readParameterGraph(fixtures / "semantics.ll", {"meaning", {0, 2}, 0, 0});
    require(named(program, "%signed_compare").operation.interpretation == IntegerInterpretation::Signed, "i64 signed compare lost");
    require(named(program, "%unsigned_compare").operation.interpretation == IntegerInterpretation::Unsigned, "same i64 unsigned compare lost");
    require(named(program, "%signed_compare32").operation.interpretation == IntegerInterpretation::Signed, "i32 signed compare lost");
    require(named(program, "%unsigned_compare32").operation.interpretation == IntegerInterpretation::Unsigned, "same i32 unsigned compare lost");
    require(named(program, "%signed_div").operation.exact, "exact flag lost");
    require(named(program, "%signed_div").operation.interpretation == IntegerInterpretation::Signed, "sdiv semantics lost");
    require(named(program, "%unsigned_div").operation.interpretation == IntegerInterpretation::Unsigned, "udiv semantics lost");
    require(named(program, "%signed_extend").operation.interpretation == IntegerInterpretation::Signed, "sext semantics lost");
    require(named(program, "%unsigned_extend").operation.interpretation == IntegerInterpretation::Unsigned, "zext semantics lost");
    require(named(program, "%signed_flag").operation.noSignedWrap, "nsw flag lost");
    require(named(program, "%unsigned_flag").operation.noUnsignedWrap, "nuw flag lost");
    require(named(program, "%both_flags").operation.noSignedWrap && named(program, "%both_flags").operation.noUnsignedWrap, "combined flags lost");
    require(named(program, "%plain_add").operation.interpretation == IntegerInterpretation::BitPattern, "plain i64 add assigned signedness");
    require(named(program, "%narrow").bitWidth == 32, "trunc result width lost");
    require(named(program, "%logical_shift").operation.interpretation == IntegerInterpretation::Unsigned, "lshr semantics lost");
    require(named(program, "%arithmetic_shift").operation.interpretation == IntegerInterpretation::Signed, "ashr semantics lost");
    require(!named(program, "%plain_add").operation.ir.empty(), "original instruction lost");
    require(named(program, "%nonnegative_extend").operation.nonNegative, "nneg flag lost");
    require(named(program, "%same_sign_compare").operation.sameSign, "samesign flag lost");
    require(named(program, "%disjoint_or").operation.disjoint, "disjoint flag lost");
    require(named(program, "%narrow_flags").operation.noSignedWrap && named(program, "%narrow_flags").operation.noUnsignedWrap, "trunc flags lost");

    const auto risks = readParameterGraph(fixtures / "semantics.ll", {"risk_cases", {0}, 0, 0});
    const auto hasRisk = [&](const std::string& label, ArithmeticRiskReason reason) {
        const auto& entries = named(risks, label).arithmeticRisks;
        return std::any_of(entries.begin(), entries.end(), [&](const auto& risk) { return risk.reason == reason; });
    };
    require(hasRisk("%add_one", ArithmeticRiskReason::SignedOverflow), "signed add risk missing");
    require(hasRisk("%add_one", ArithmeticRiskReason::UnsignedWrap), "unsigned add risk missing");
    require(named(risks, "%add_zero").arithmeticRisks.empty(), "adding zero must not warn");
    require(hasRisk("%subtract", ArithmeticRiskReason::UnsignedUnderflow), "unsigned subtraction risk missing");
    require(named(risks, "%subtract_same").arithmeticRisks.empty(), "subtracting the same SSA value must not warn");
    require(hasRisk("%multiply_two", ArithmeticRiskReason::UnsignedWrap), "unsigned multiplication risk missing");
    require(named(risks, "%multiply_one").arithmeticRisks.empty(), "multiplying by one must not warn");
    require(hasRisk("%divide", ArithmeticRiskReason::DivideByZero), "possible division by zero missing");
    require(!hasRisk("%divide_const", ArithmeticRiskReason::DivideByZero), "known nonzero divisor warned");
    require(hasRisk("%signed_divide", ArithmeticRiskReason::SignedDivisionOverflow), "signed division overflow missing");
    require(named(risks, "%signed_divide_const").arithmeticRisks.empty(), "known safe signed divisor warned");
    require(hasRisk("%unsigned_constant_wrap", ArithmeticRiskReason::UnsignedWrap), "constant unsigned wrap missing");
    require(!hasRisk("%unsigned_constant_wrap", ArithmeticRiskReason::SignedOverflow), "safe signed constant add warned");
    require(hasRisk("%signed_constant_overflow", ArithmeticRiskReason::SignedOverflow), "constant signed overflow missing");
    require(!hasRisk("%signed_constant_overflow", ArithmeticRiskReason::UnsignedWrap), "safe unsigned constant add warned");
}

void checkContexts(const std::filesystem::path& fixtures) {
    const auto program = readParameterGraph(fixtures / "contexts.ll", {"identity", {0}, 1, 1});
    require(program.query.parameters.size() == 2, "different calls collapsed");
    std::set<std::string> contexts;
    for (auto seed : program.query.parameters) {
        contexts.insert(program.graph.node(seed).context);
        const auto slice = buildSlice(program.graph, {{seed}, 1, 0});
        unsigned pairParameters = 0;
        for (const auto& reached : slice.upstream.nodes) {
            const auto& node = program.graph.node(reached.node);
            if (node.function == "pair" && node.kind == NodeKind::Parameter) ++pairParameters;
        }
        require(pairParameters == 1, "arguments from different call sites mixed");
    }
    require(contexts.size() == 2, "call context identity lost");
    const auto memory = readParameterGraph(fixtures / "contexts.ll", {"memory", {1}, 0, 0});
    require(named(memory, "%loaded").unresolved.empty(), "entry pointer load was not represented");
    const auto indirect = readParameterGraph(fixtures / "contexts.ll", {"indirect", {0}, 0, 0});
    require(!indirect.diagnostics.empty(), "indirect call not reported");
    const auto recursive = readParameterGraph(fixtures / "contexts.ll", {"recursive", {0}, 0, 0});
    require(!recursive.diagnostics.empty(), "recursive expansion did not retain boundary");
    const auto external = readParameterGraph(fixtures / "contexts.ll", {"external_user", {0}, 0, 1});
    require(!external.diagnostics.empty(), "external definition not reported");
    const auto pointerScope = readParameterGraph(fixtures / "contexts.ll", {"memory", {0}, 0, 0});
    require(pointerScope.graph.node(pointerScope.query.parameters.front()).bitWidth == 64,
            "pointer parameter did not receive target-width representation");
    mustFail([&] { readParameterGraph(fixtures / "overloads.ll", {"f", {0}, 0, 0}); }, "ambiguous C++ overload accepted");
    const auto overload = readParameterGraph(fixtures / "overloads.ll", {"f(int)", {0}, 0, 0});
    require(overload.graph.node(overload.query.parameters.front()).bitWidth == 32, "full C++ signature not resolved");
    const auto exact = readParameterGraph(fixtures / "overloads.ll", {"_Z1fx", {0}, 0, 0});
    require(exact.graph.node(exact.query.parameters.front()).bitWidth == 64, "exact LLVM symbol not resolved");
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 5) throw std::invalid_argument("expected ll, bc, fixture directory and nodebug ll");
        checkCppChain(argv[1], argv[2]);
        checkSemantics(argv[3]);
        checkContexts(argv[3]);
        const auto noDebug = readParameterGraph(argv[4], {"B", {0}, 1, 1});
        require(noDebug.query.parameters.size() == 1, "C++ name resolution requires debug info");
        std::cout << "LLVM reader: real C++, ll/bc, depths, contexts, integer semantics and boundaries passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
