#include "valuetrace/RiskVerifier.h"
#include <iostream>
#include <stdexcept>

using namespace valuetrace;
namespace {
void require(bool ok, const std::string& why) { if (!ok) throw std::runtime_error(why); }
ParameterQuery from(std::string function, std::string parameter, unsigned down) {
    ParameterQuery q;
    q.function = std::move(function);
    q.parameterNames = {std::move(parameter)};
    q.upDepth = 0;
    q.downDepth = down;
    return q;
}
const ArithmeticRisk& unsignedWrapIn(const RiskVerificationReport& r, const std::string& function) {
    for (NodeId id = 0; id < r.program.graph.size(); ++id) {
        const auto& node = r.program.graph.node(id);
        if (node.function != function) continue;
        for (const auto& risk : node.arithmeticRisks)
            if (risk.reason == ArithmeticRiskReason::UnsignedWrap && risk.verification) return risk;
    }
    throw std::runtime_error("expected UInt32 addition risk in " + function + " was not connected to the selected parameter");
}
bool mentions(const std::vector<std::string>& lines, const std::string& text) {
    for (const auto& line : lines) if (line.find(text) != std::string::npos) return true;
    return false;
}
void case1(const std::filesystem::path& file) {
    const auto r = verifyArithmeticRisks(file, from("A", "value", 2));
    const auto& risk = unsignedWrapIn(r, "C");
    require(risk.status == ReportStatus::BoundaryTriggerable, "01: pointer output did not reach C's addition");
    const auto& evidence = *risk.verification;
    require(mentions(evidence.path, "B") && mentions(evidence.path, "C") && evidence.inputs.size() == 1,
            "01: witness lacks B -> C path or input");
    require(mentions(evidence.memoryEvidence, "offset=0") &&
            mentions(evidence.memoryEvidence, "size=4") &&
            mentions(evidence.memoryEvidence, "last-write=B") &&
            mentions(evidence.memoryEvidence, "update=strong"),
            "01: B's strong write to ctx.len was not recorded");
    require(std::stoull(evidence.operands.at(0).unsignedValue) == std::stoull(evidence.inputs.at(0).unsignedValue),
            "01: B's final ctx.len write was not used by C");
}
void escapedGlobal(const std::filesystem::path& file) {
    const auto r = verifyArithmeticRisks(file, from("EscapeGlobal", "value", 1));
    const auto& risk = unsignedWrapIn(r, "EscapeGlobal");
    require(risk.status == ReportStatus::Unknown,
            "07: escaped address was promoted to a definite trigger");
    require(mentions(risk.verification->unmodeledDependencies, "地址已逃逸（待 6b）"),
            "07: escaped load did not become a full-width arbitrary value");
}
void case2(const std::filesystem::path& file) {
    const auto r = verifyArithmeticRisks(file, from("FieldCheck", "count", 2));
    require(r.candidates == 0, "02: updating count incorrectly influenced len");
    bool checkedLoad = false;
    for (NodeId id = 0; id < r.program.graph.size(); ++id) {
        const auto& node = r.program.graph.node(id);
        if (node.function == "UseLen" && node.operation.opcode == "load") {
            checkedLoad = true;
            require(node.unresolved.empty(), "02: len load was skipped, so field independence is not demonstrated");
        }
        if (node.function == "UseLen" && node.operation.opcode == "add")
            throw std::runtime_error("02: count and len field offsets were merged");
    }
    require(checkedLoad, "02: the len read was absent from the inspected scope");
}
void case3(const std::filesystem::path& file) {
    const auto r = verifyArithmeticRisks(file, from("ConditionalRead", "value", 1));
    const auto& risk = unsignedWrapIn(r, "ConditionalRead");
    require(risk.status == ReportStatus::Unknown, "03: unresolved two-target alias was promoted to a definite result");
    const auto& e = *risk.verification;
    require(mentions(e.notes, "weak-update") && mentions(e.notes, "alias-choice") &&
            mentions(e.constraints, "chooseFirst"), "03: old/new values or conditional alias choice missing");
}
void case4(const std::filesystem::path& file) {
    const auto r = verifyArithmeticRisks(file, from("ExternalPointer", "value", 1));
    const auto& risk = unsignedWrapIn(r, "ExternalPointer");
    require(risk.status == ReportStatus::Unknown, "04: unknown external pointer was treated as one known object");
    require(mentions(r.program.diagnostics, "instrumentation-suggestion") &&
            mentions(r.program.diagnostics, "WriteUnknown"), "04: missing pointer-identity observation suggestion");
}
void case5(const std::filesystem::path& file) {
    const auto r = verifyArithmeticRisks(file, from("DynamicIndex", "value", 1));
    const auto& risk = unsignedWrapIn(r, "DynamicIndex");
    require(risk.status == ReportStatus::Unknown, "05: variable array index was treated as exact");
    const auto& e = *risk.verification;
    require(mentions(e.notes, "weak-update") && mentions(e.constraints, "index"),
            "05: weak update did not preserve old value and index == 0 branch");
}
void case6(const std::filesystem::path& file) {
    const auto r = verifyArithmeticRisks(file, from("MemcpyField", "value", 1));
    const auto& risk = unsignedWrapIn(r, "MemcpyField");
    require(risk.status == ReportStatus::BoundaryTriggerable, "06: memcpy field transfer did not reach later addition");
    const auto& e = *risk.verification;
    require(mentions(e.notes, "memcpy") && e.inputs.size() == 1 &&
            std::stoull(e.operands.at(0).unsignedValue) == std::stoull(e.inputs.at(0).unsignedValue),
            "06: four-byte src.len -> dst.len summary missing or unrelated field changed");
}
}
int main(int argc, char** argv) {
    try {
        require(argc == 3 || argc == 7, "expected stage 6a fixtures or six compiled memory acceptance fixtures");
        case1(argv[1]);
        if (argc == 3) { escapedGlobal(argv[2]); std::cout << "Stage 6a pointer acceptance passed\n"; return 0; }
        case2(argv[2]); case3(argv[3]);
        case4(argv[4]); case5(argv[5]); case6(argv[6]);
        std::cout << "Six memory acceptance cases passed\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
