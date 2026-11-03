#include "RiskConditions.h"
#include <stdexcept>

namespace valuetrace::symbolic {

z3::expr riskCondition(const llvm::Instruction& instruction, ArithmeticRiskReason reason,
                       std::span<const Value> operands) {
    if (operands.empty()) throw std::invalid_argument("risk requires an operand");
    const auto& left = operands.front();
    const auto& a = left.bits;
    const auto opcode = instruction.getOpcode();
    const auto width = a.get_sort().bv_size();
    if (reason == ArithmeticRiskReason::SignedTruncationLoss || reason == ArithmeticRiskReason::UnsignedTruncationLoss) {
        if (opcode != llvm::Instruction::Trunc) throw std::invalid_argument("truncation loss requires trunc");
        const auto narrow = cast(instruction, left).bits;
        const auto extra = width - narrow.get_sort().bv_size();
        return a != (reason == ArithmeticRiskReason::SignedTruncationLoss ? z3::sext(narrow, extra) : z3::zext(narrow, extra));
    }
    if (operands.size() != 2) throw std::invalid_argument("binary risk requires two operands");
    const auto& right = operands[1];
    const auto& b = right.bits;
    const bool shift = opcode == llvm::Instruction::Shl || opcode == llvm::Instruction::LShr || opcode == llvm::Instruction::AShr;
    if (reason == ArithmeticRiskReason::InvalidShiftAmount) {
        if (!shift) throw std::invalid_argument("invalid shift amount requires a shift instruction");
        // A negative two's-complement amount is also >= width under unsigned comparison.
        return z3::uge(b, a.ctx().bv_val(width, width));
    }
    if (reason == ArithmeticRiskReason::DivideByZero) {
        if (opcode != llvm::Instruction::UDiv && opcode != llvm::Instruction::SDiv &&
            opcode != llvm::Instruction::URem && opcode != llvm::Instruction::SRem) {
            throw std::invalid_argument("divide-by-zero predicate requires division or remainder");
        }
        return b == 0;
    }
    if (reason == ArithmeticRiskReason::SignedDivisionOverflow) {
        if (opcode != llvm::Instruction::SDiv && opcode != llvm::Instruction::SRem) throw std::invalid_argument("signed division overflow requires sdiv/srem");
        const auto minimum = z3::shl(a.ctx().bv_val(1, width), a.ctx().bv_val(width - 1, width));
        return a == minimum && b == -1;
    }
    if (reason == ArithmeticRiskReason::UnsignedUnderflow) {
        if (opcode != llvm::Instruction::Sub) throw std::invalid_argument("unsigned underflow predicate requires sub");
        return z3::ult(a, b);
    }
    if (reason != ArithmeticRiskReason::SignedOverflow && reason != ArithmeticRiskReason::UnsignedWrap) {
        throw std::invalid_argument("automatic predicate not implemented for this risk reason");
    }
    const bool isSigned = reason == ArithmeticRiskReason::SignedOverflow;
    if (opcode != llvm::Instruction::Add && opcode != llvm::Instruction::Sub &&
        opcode != llvm::Instruction::Mul && opcode != llvm::Instruction::Shl)
        throw std::invalid_argument("overflow predicate requires a supported arithmetic operation");
    const auto overflow = overflows(instruction, a, b, isSigned);
    return opcode == llvm::Instruction::Shl ? z3::ult(b, a.ctx().bv_val(width, width)) && overflow : overflow;
}

} // namespace valuetrace::symbolic
