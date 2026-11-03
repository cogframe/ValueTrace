#include "IntegerExpressions.h"
#include "RiskConditions.h"
#include "valuetrace/ArithmeticReport.h"
#include "valuetrace/RiskVerifier.h"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace valuetrace;

namespace {
void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

llvm::Instruction& operation(llvm::Module& module, const char* functionName) {
    auto* function = module.getFunction(functionName);
    require(function != nullptr, std::string("missing fixture: ") + functionName);
    for (auto& block : *function) for (auto& instruction : block)
        if (instruction.getName() == "r") return instruction;
    throw std::runtime_error(std::string("missing operation in ") + functionName);
}

bool holds(const z3::expr& condition) {
    z3::solver solver(condition.ctx());
    solver.add(condition);
    const auto result = solver.check();
    require(result != z3::unknown, "Z3 could not decide a concrete boundary predicate");
    return result == z3::sat;
}

void checkBoundary(llvm::Module& module, const char* function, ArithmeticRiskReason reason,
                   const char* failingLeft, const char* failingRight,
                   const char* safeLeft, const char* safeRight, bool failingValueDefined) {
    auto& instruction = operation(module, function);
    z3::context context;
    const auto width = instruction.getOperand(0)->getType()->getIntegerBitWidth();
    const auto check = [&](const char* leftText, const char* rightText, bool expectedRisk, bool expectedDefined) {
        std::vector<symbolic::Value> operands;
        operands.push_back({context.bv_val(leftText, width), context.bool_val(true), context.bool_val(true)});
        if (rightText) operands.push_back({context.bv_val(rightText, width), context.bool_val(true), context.bool_val(true)});
        const auto risk = symbolic::riskCondition(instruction, reason, operands);
        const auto result = rightText ? symbolic::binary(instruction, operands[0], operands[1]) :
                                  symbolic::cast(instruction, operands[0]);
        require(holds(risk) == expectedRisk && holds(result.valid) == expectedDefined,
                std::string(function) + ": incorrect risk or poison boundary");
    };
    check(failingLeft, failingRight, true, failingValueDefined);
    check(safeLeft, safeRight, false, true);
}

void checkCast(llvm::Module& module, const char* function, const char* input,
               const char* expectedBits, bool expectedDefined) {
    auto& instruction = operation(module, function);
    z3::context context;
    const auto width = instruction.getOperand(0)->getType()->getIntegerBitWidth();
    const symbolic::Value source{context.bv_val(input, width), context.bool_val(true), context.bool_val(true)};
    const auto value = symbolic::cast(instruction, source);
    require(holds(value.bits == context.bv_val(expectedBits, value.bits.get_sort().bv_size())) &&
            holds(value.valid) == expectedDefined, std::string(function) + ": extension differs from LangRef");
}

void checkExact(llvm::Module& module, const char* function, const char* failingLeft,
                const char* safeLeft, const char* second) {
    auto& instruction = operation(module, function);
    z3::context context;
    const auto width = instruction.getType()->getIntegerBitWidth();
    const auto calculate = [&](const char* first) {
        const symbolic::Value a{context.bv_val(first, width), context.bool_val(true), context.bool_val(true)};
        const symbolic::Value b{context.bv_val(second, width), context.bool_val(true), context.bool_val(true)};
        return symbolic::binary(instruction, a, b);
    };
    require(!holds(calculate(failingLeft).valid) && holds(calculate(safeLeft).valid),
            std::string(function) + ": exact flag boundary is wrong");
}

void checkBinaryFlag(llvm::Module& module, const char* function, const char* failingLeft,
                     const char* failingRight, const char* safeLeft, const char* safeRight) {
    auto& instruction = operation(module, function);
    z3::context context;
    const auto width = instruction.getOperand(0)->getType()->getIntegerBitWidth();
    const auto valid = [&](const char* left, const char* right) {
        const symbolic::Value a{context.bv_val(left, width), context.bool_val(true), context.bool_val(true)};
        const symbolic::Value b{context.bv_val(right, width), context.bool_val(true), context.bool_val(true)};
        return llvm::isa<llvm::ICmpInst>(instruction) ? symbolic::compare(instruction, a, b).valid :
                                                     symbolic::binary(instruction, a, b).valid;
    };
    require(!holds(valid(failingLeft, failingRight)) && holds(valid(safeLeft, safeRight)),
            std::string(function) + ": flag poison boundary is wrong");
}

std::string report(const std::filesystem::path& file, const char* function) {
    ParameterQuery query;
    query.function = function;
    query.parameters = {0};
    query.upDepth = 0;
    query.downDepth = 0;
    const auto verified = verifyArithmeticRisks(file, query);
    std::ostringstream output;
    writeArithmeticReport(output, verified.program.graph, verified.slice, ReportFilter::forLevel(5));
    return output.str();
}
} // namespace

int main(int argc, char** argv) {
    try {
        require(argc == 2, "expected LLVM IR fixture path");
        llvm::LLVMContext llvmContext;
        llvm::SMDiagnostic diagnostic;
        auto module = llvm::parseIRFile(argv[1], diagnostic, llvmContext);
        require(module != nullptr, "could not parse poison boundary fixture");
        using R = ArithmeticRiskReason;
        checkBoundary(*module, "add_plain", R::SignedOverflow, "127", "1", "126", "1", true);
        checkBoundary(*module, "add_nsw", R::SignedOverflow, "127", "1", "126", "1", false);
        checkBoundary(*module, "add_plain", R::UnsignedWrap, "255", "1", "254", "1", true);
        checkBoundary(*module, "add_nuw", R::UnsignedWrap, "255", "1", "254", "1", false);
        checkBoundary(*module, "add_nuw", R::SignedOverflow, "127", "1", "126", "1", true);
        checkBoundary(*module, "add_nsw", R::UnsignedWrap, "255", "1", "254", "1", true);
        checkBoundary(*module, "add64_nsw", R::SignedOverflow,
                      "9223372036854775807", "1", "9223372036854775806", "1", false);
        checkBoundary(*module, "sub_plain", R::SignedOverflow, "128", "1", "129", "1", true);
        checkBoundary(*module, "sub_nsw", R::SignedOverflow, "128", "1", "129", "1", false);
        checkBoundary(*module, "sub_plain", R::UnsignedUnderflow, "0", "1", "1", "1", true);
        checkBoundary(*module, "sub_nuw", R::UnsignedUnderflow, "0", "1", "1", "1", false);
        checkBoundary(*module, "mul_plain", R::SignedOverflow, "64", "2", "63", "2", true);
        checkBoundary(*module, "mul_nsw", R::SignedOverflow, "64", "2", "63", "2", false);
        checkBoundary(*module, "mul_plain", R::UnsignedWrap, "128", "2", "127", "2", true);
        checkBoundary(*module, "mul_nuw", R::UnsignedWrap, "128", "2", "127", "2", false);
        checkBoundary(*module, "shl_plain", R::SignedOverflow, "64", "1", "63", "1", true);
        checkBoundary(*module, "shl_nsw", R::SignedOverflow, "64", "1", "63", "1", false);
        checkBoundary(*module, "shl_plain", R::UnsignedWrap, "128", "1", "127", "1", true);
        checkBoundary(*module, "shl_nuw", R::UnsignedWrap, "128", "1", "127", "1", false);
        for (const auto* name : {"shl_plain", "lshr_plain", "ashr_plain"})
            checkBoundary(*module, name, R::InvalidShiftAmount, "1", "8", "1", "7", false);
        for (const auto* name : {"udiv_plain", "sdiv_plain", "urem_plain", "srem_plain"})
            checkBoundary(*module, name, R::DivideByZero, "1", "0", "1", "1", false);
        for (const auto* name : {"sdiv_plain", "srem_plain"})
            checkBoundary(*module, name, R::SignedDivisionOverflow, "128", "255", "129", "255", false);
        checkBoundary(*module, "trunc_plain", R::SignedTruncationLoss, "128", nullptr, "127", nullptr, true);
        checkBoundary(*module, "trunc_nsw", R::SignedTruncationLoss, "128", nullptr, "127", nullptr, false);
        checkBoundary(*module, "trunc_plain", R::UnsignedTruncationLoss, "256", nullptr, "255", nullptr, true);
        checkBoundary(*module, "trunc_nuw", R::UnsignedTruncationLoss, "256", nullptr, "255", nullptr, false);
        checkCast(*module, "zext_plain", "255", "255", true);
        checkCast(*module, "zext_nneg", "128", "128", false);
        checkCast(*module, "zext_nneg", "127", "127", true);
        checkCast(*module, "sext_plain", "128", "65408", true);
        checkExact(*module, "udiv_exact", "5", "4", "2");
        checkExact(*module, "sdiv_exact", "5", "4", "2");
        checkExact(*module, "lshr_exact", "3", "2", "1");
        checkExact(*module, "ashr_exact", "3", "2", "1");
        checkBinaryFlag(*module, "or_disjoint", "1", "1", "1", "2");
        checkBinaryFlag(*module, "icmp_samesign", "255", "1", "255", "254");
        require(report(argv[1], "add_nsw").find("Effect: poison (violated target flag nsw)") != std::string::npos,
                "nsw report does not explain poison");
        require(report(argv[1], "add_nuw").find("Effect: poison (violated target flag nuw)") != std::string::npos,
                "nuw report does not explain poison");
        require(report(argv[1], "add_plain").find("Effect: defined modulo-2^8 bit pattern") != std::string::npos,
                "unflagged report does not explain modular wrap");
        require(report(argv[1], "shl_plain").find("Effect: poison (shift amount is at least the bit width)") != std::string::npos,
                "invalid shift report does not explain poison");
        require(report(argv[1], "sdiv_plain").find("Effect: undefined behavior at division or remainder") != std::string::npos,
                "signed division report does not explain undefined behavior");
        std::cout << "Poison and arithmetic boundaries match the LLVM instruction semantics\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
