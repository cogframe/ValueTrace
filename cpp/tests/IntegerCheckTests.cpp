#include "valuetrace/RiskVerifier.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>

using namespace valuetrace;
extern "C" unsigned short Narrow32(unsigned);
extern "C" unsigned Narrow64(unsigned long long);
extern "C" unsigned Shift32(unsigned, unsigned);
extern "C" unsigned long long Shift64(unsigned long long, unsigned long long);
namespace {
void require(bool ok, const std::string& why) { if (!ok) throw std::runtime_error(why); }
ParameterQuery query(std::string function, unsigned parameter = 0, unsigned up = 0) {
    ParameterQuery q; q.function = std::move(function); q.parameters = {parameter}; q.upDepth = up; q.downDepth = 0; return q;
}
const ArithmeticRisk& risk(const RiskVerificationReport& report, ArithmeticRiskReason reason) {
    for (NodeId id = 0; id < report.program.graph.size(); ++id)
        for (const auto& r : report.program.graph.node(id).arithmeticRisks)
            if (r.reason == reason && r.verification) return r;
    throw std::runtime_error(std::string("missing verified risk: ") + arithmeticRiskReasonName(reason));
}
void expect(const RiskVerificationReport& report, ArithmeticRiskReason reason, ReportStatus status) {
    const auto& r = risk(report, reason);
    require(r.status == status, std::string(arithmeticRiskReasonName(reason)) + ": " + r.verification->detail);
    require(!r.verification->sourceClosed, "source closure was invented");
}
void constants(const std::filesystem::path& file) {
    const auto check = [&](const char* function, const char* label, ArithmeticRiskReason reason, bool expected) {
        const auto imported = readParameterGraph(file, query(function));
        bool found = false, warned = false;
        for (NodeId id = 0; id < imported.graph.size(); ++id) {
            const auto& n = imported.graph.node(id);
            if (n.function != function || n.label != label) continue;
            found = true;
            for (const auto& r : n.arithmeticRisks) {
                if (r.reason == reason) warned = true;
                require(r.status == ReportStatus::Unverified, "trace/import verified a candidate");
            }
        }
        require(found && warned == expected, std::string("constant classification wrong: ") + function + label);
    };
    check("negative_one", "%r", ArithmeticRiskReason::SignedTruncationLoss, false);
    check("negative_one", "%r", ArithmeticRiskReason::UnsignedTruncationLoss, true);
    check("positive_max", "%r", ArithmeticRiskReason::SignedTruncationLoss, true);
    check("positive_max", "%r", ArithmeticRiskReason::UnsignedTruncationLoss, false);
    check("safe_constant", "%r", ArithmeticRiskReason::SignedTruncationLoss, false);
    check("safe_constant", "%r", ArithmeticRiskReason::UnsignedTruncationLoss, false);
    for (const auto* label : {"%safe", "%zero", "%identity"}) {
        check("constant_shift", label, ArithmeticRiskReason::SignedOverflow, false);
        check("constant_shift", label, ArithmeticRiskReason::UnsignedWrap, false);
        check("constant_shift", label, ArithmeticRiskReason::InvalidShiftAmount, false);
    }
    check("constant_shift", "%wrap", ArithmeticRiskReason::UnsignedWrap, true);
    check("constant_shift", "%wrap", ArithmeticRiskReason::SignedOverflow, false);
    check("constant_shift", "%bad", ArithmeticRiskReason::InvalidShiftAmount, true);
}
void irCases(const std::filesystem::path& file) {
    constexpr auto sat = ReportStatus::BoundaryTriggerable, unsat = ReportStatus::ScopedUnreachable;
    for (const auto* name : {"narrow32", "narrow64", "narrow1", "late_narrow_guard"}) {
        const auto r = verifyArithmeticRisks(file, query(name));
        for (auto reason : {ArithmeticRiskReason::SignedTruncationLoss, ArithmeticRiskReason::UnsignedTruncationLoss}) {
            expect(r, reason, sat);
            const auto& e = *risk(r, reason).verification;
            require(e.operands.size() == 1 && !e.resultBitPattern.empty() && !e.path.empty(), "unary witness incomplete");
            require(e.resultIsPoison == (std::string(name) == "narrow64"), "target trunc flags were assumed or lost");
        }
    }
    const auto u = verifyArithmeticRisks(file, query("unsigned_guard"));
    expect(u, ArithmeticRiskReason::UnsignedTruncationLoss, unsat);
    expect(u, ArithmeticRiskReason::SignedTruncationLoss, sat);
    const auto s = verifyArithmeticRisks(file, query("signed_guard"));
    expect(s, ArithmeticRiskReason::SignedTruncationLoss, unsat);
    expect(s, ArithmeticRiskReason::UnsignedTruncationLoss, sat);
    const auto entryNarrow = verifyArithmeticRisks(file, query("unknown_narrow"));
    expect(entryNarrow, ArithmeticRiskReason::UnsignedTruncationLoss, ReportStatus::Unknown);
    require(!risk(entryNarrow, ArithmeticRiskReason::UnsignedTruncationLoss).verification->memoryEvidence.empty(),
            "unmodeled entry memory lacks a memory location");
    for (const auto* name : {"left32", "left64", "logical32", "arithmetic64", "late_shift_guard"}) {
        const auto r = verifyArithmeticRisks(file, query(name, 1));
        expect(r, ArithmeticRiskReason::InvalidShiftAmount, sat);
        const auto& e = *risk(r, ArithmeticRiskReason::InvalidShiftAmount).verification;
        const auto& amount = e.operands.at(1);
        require(std::stoull(amount.unsignedValue) >= amount.bitWidth && e.resultIsPoison &&
                e.resultBitPattern.empty() && !e.noDefinedResult, "invalid shift witness treated as a defined value or immediate UB");
        if (std::string(name).starts_with("left")) {
            for (auto reason : {ArithmeticRiskReason::SignedOverflow, ArithmeticRiskReason::UnsignedWrap}) {
                expect(r, reason, sat);
                const auto& overflow = *risk(r, reason).verification;
                require(std::stoull(overflow.operands.at(1).unsignedValue) < amount.bitWidth && overflow.resultIsPoison,
                        "shift overflow used illegal amount or assumed own flags");
            }
        }
    }
    const auto amount = verifyArithmeticRisks(file, query("amount_guard", 1));
    expect(amount, ArithmeticRiskReason::InvalidShiftAmount, unsat);
    expect(amount, ArithmeticRiskReason::UnsignedWrap, sat); // Amount check alone does not prevent value overflow.
    const auto full = verifyArithmeticRisks(file, query("fully_guarded", 1));
    for (auto reason : {ArithmeticRiskReason::InvalidShiftAmount, ArithmeticRiskReason::SignedOverflow, ArithmeticRiskReason::UnsignedWrap}) expect(full, reason, unsat);
    for (const auto* name : {"negative_shift", "only_upper_guard"}) {
        const auto r = verifyArithmeticRisks(file, query(name));
        expect(r, ArithmeticRiskReason::InvalidShiftAmount, sat);
        require(std::stoll(risk(r, ArithmeticRiskReason::InvalidShiftAmount).verification->operands.at(1).signedValue) < 0,
                "negative shift escaped detection");
    }
    expect(verifyArithmeticRisks(file, query("unknown_shift")), ArithmeticRiskReason::InvalidShiftAmount, ReportStatus::Unknown);
    expect(verifyArithmeticRisks(file, query("unknown_left")), ArithmeticRiskReason::InvalidShiftAmount, sat);
    const auto prior = verifyArithmeticRisks(file, query("prior_poison"));
    expect(prior, ArithmeticRiskReason::InvalidShiftAmount, sat);
    expect(prior, ArithmeticRiskReason::UnsignedTruncationLoss, unsat);
    VerificationOptions tiny; tiny.maxSteps = 1;
    expect(verifyArithmeticRisks(file, query("unsigned_guard"), tiny), ArithmeticRiskReason::UnsignedTruncationLoss, ReportStatus::Unknown);
    constants(file);
}
void cppCases(const std::filesystem::path& ll, const std::filesystem::path& bc) {
    for (const auto& file : {ll, bc}) {
        for (const auto* name : {"Narrow32", "Narrow64"}) {
            auto q = query(name, 0, 1); q.parameters.clear(); q.parameterNames = {"value"};
            const auto guarded = verifyArithmeticRisks(file, q);
            expect(guarded, ArithmeticRiskReason::UnsignedTruncationLoss, ReportStatus::ScopedUnreachable);
            q.upDepth = 0;
            const auto boundary = verifyArithmeticRisks(file, q);
            expect(boundary, ArithmeticRiskReason::UnsignedTruncationLoss, ReportStatus::BoundaryTriggerable);
            const auto& e = *risk(boundary, ArithmeticRiskReason::UnsignedTruncationLoss).verification;
            const auto input = std::stoull(e.operands.front().unsignedValue);
            const auto actual = std::string(name) == "Narrow32" ? Narrow32(static_cast<unsigned>(input)) : Narrow64(input);
            require(actual == std::stoull(e.resultBitPattern), "real C++ truncation witness failed replay");
        }
        for (const auto* name : {"Shift32", "Shift64"}) {
            const auto r = verifyArithmeticRisks(file, query(name, 1));
            expect(r, ArithmeticRiskReason::InvalidShiftAmount, ReportStatus::BoundaryTriggerable);
            const auto& e = *risk(r, ArithmeticRiskReason::UnsignedWrap).verification;
            const auto value = std::stoull(e.operands[0].unsignedValue), amount = std::stoull(e.operands[1].unsignedValue);
            require(amount < e.operands[0].bitWidth, "invalid amount sent to native replay");
            const auto actual = std::string(name) == "Shift32" ? Shift32(static_cast<unsigned>(value), static_cast<unsigned>(amount)) : Shift64(value, amount);
            require(actual == std::stoull(e.resultBitPattern), "real C++ left-shift wrap witness failed replay");
        }
        expect(verifyArithmeticRisks(file, query("Shift32", 1, 1)), ArithmeticRiskReason::InvalidShiftAmount, ReportStatus::ScopedUnreachable);
    }
}
}
int main(int argc, char** argv) {
    try {
        require(argc == 4, "expected hand IR, C++ IR and bitcode");
        irCases(argv[1]); cppCases(argv[2], argv[3]);
        std::cout << "Truncation and shift verification passed\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
