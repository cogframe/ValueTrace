#include "IntegerExpressions.h"

#include <llvm/ADT/APInt.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Operator.h>
#include <llvm/Support/raw_ostream.h>

#include <cctype>
#include <algorithm>
#include <stdexcept>

namespace valuetrace::symbolic {
namespace {

z3::expr booleanBits(const z3::expr& boolean) {
    return z3::ite(boolean, boolean.ctx().bv_val(1, 1), boolean.ctx().bv_val(0, 1));
}

z3::expr minimum(const z3::expr& value) {
    const auto width = value.get_sort().bv_size();
    return z3::shl(value.ctx().bv_val(1, width), value.ctx().bv_val(width - 1, width));
}

} // namespace

std::string instructionText(const llvm::Instruction& instruction) {
    std::string text;
    llvm::raw_string_ostream stream(text);
    instruction.print(stream);
    return text;
}

z3::expr overflows(const llvm::Instruction& instruction, const z3::expr& left,
                   const z3::expr& right, bool isSigned) {
    const auto width = left.get_sort().bv_size();
    if (right.get_sort().bv_size() != width) throw std::invalid_argument("overflow operands must have equal widths");
    const auto wideWidth = width * 2;
    auto& context = left.ctx();
    const auto a = isSigned ? z3::sext(left, width) : z3::zext(left, width);
    const auto b = isSigned ? z3::sext(right, width) : z3::zext(right, width);
    z3::expr mathematical = a;
    switch (instruction.getOpcode()) {
    case llvm::Instruction::Add: mathematical = a + b; break;
    case llvm::Instruction::Sub: mathematical = a - b; break;
    case llvm::Instruction::Mul: mathematical = a * b; break;
    case llvm::Instruction::Shl: mathematical = z3::shl(a, z3::zext(right, width)); break;
    default: throw std::invalid_argument("overflow requires add, sub, mul, or shl");
    }
    const auto one = context.bv_val(1, wideWidth);
    const auto signedMagnitude = z3::shl(one, context.bv_val(width - 1, wideWidth));
    if (isSigned) return mathematical < -signedMagnitude || mathematical > signedMagnitude - one;
    const auto unsignedLimit = z3::shl(one, context.bv_val(width, wideWidth)) - one;
    return z3::ugt(mathematical, unsignedLimit);
}

Value binary(const llvm::Instruction& instruction, const Value& left, const Value& right,
             bool ignoreNoWrap) {
    const auto& a = left.bits;
    const auto& b = right.bits;
    auto& context = a.ctx();
    const auto width = a.get_sort().bv_size();
    auto result = a;
    auto valid = left.valid && right.valid;
    const auto opcode = instruction.getOpcode();
    switch (opcode) {
    case llvm::Instruction::Add: result = a + b; break;
    case llvm::Instruction::Sub: result = a - b; break;
    case llvm::Instruction::Mul: result = a * b; break;
    case llvm::Instruction::And: result = a & b; break;
    case llvm::Instruction::Or: result = a | b; break;
    case llvm::Instruction::Xor: result = a ^ b; break;
    case llvm::Instruction::Shl: result = z3::shl(a, b); valid = valid && z3::ult(b, context.bv_val(width, width)); break;
    case llvm::Instruction::LShr: result = z3::lshr(a, b); valid = valid && z3::ult(b, context.bv_val(width, width)); break;
    case llvm::Instruction::AShr: result = z3::ashr(a, b); valid = valid && z3::ult(b, context.bv_val(width, width)); break;
    case llvm::Instruction::UDiv: result = z3::udiv(a, b); valid = valid && b != 0; break;
    case llvm::Instruction::URem: result = z3::urem(a, b); valid = valid && b != 0; break;
    case llvm::Instruction::SDiv: result = a / b; valid = valid && b != 0 && !(a == minimum(a) && b == -1); break;
    case llvm::Instruction::SRem: result = z3::srem(a, b); valid = valid && b != 0 && !(a == minimum(a) && b == -1); break;
    default: throw std::invalid_argument("unsupported integer binary instruction");
    }
    if (const auto* flags = llvm::dyn_cast<llvm::OverflowingBinaryOperator>(&instruction); flags && !ignoreNoWrap) {
        if (flags->hasNoSignedWrap()) valid = valid && !overflows(instruction, a, b, true);
        if (flags->hasNoUnsignedWrap()) valid = valid && !overflows(instruction, a, b, false);
    }
    if (const auto* flags = llvm::dyn_cast<llvm::PossiblyExactOperator>(&instruction); flags && flags->isExact()) {
        if (opcode == llvm::Instruction::SDiv) valid = valid && z3::srem(a, b) == 0;
        else if (opcode == llvm::Instruction::UDiv) valid = valid && z3::urem(a, b) == 0;
        else valid = valid && z3::shl(result, b) == a;
    }
    if (const auto* flags = llvm::dyn_cast<llvm::PossiblyDisjointInst>(&instruction); flags && flags->isDisjoint()) {
        valid = valid && (a & b) == 0;
    }
    return {result, valid, left.known && right.known, mergeDependencies(left, right)};
}

Value cast(const llvm::Instruction& instruction, const Value& source, bool ignoreNoWrap) {
    const auto before = source.bits.get_sort().bv_size();
    const auto after = instruction.getType()->getIntegerBitWidth();
    auto result = source.bits;
    auto valid = source.valid;
    if (instruction.getOpcode() == llvm::Instruction::Trunc) {
        result = source.bits.extract(after - 1, 0);
        const auto& flags = llvm::cast<llvm::TruncInst>(instruction);
        if (!ignoreNoWrap && flags.hasNoSignedWrap()) valid = valid && z3::sext(result, before - after) == source.bits;
        if (!ignoreNoWrap && flags.hasNoUnsignedWrap()) valid = valid && z3::zext(result, before - after) == source.bits;
    } else if (instruction.getOpcode() == llvm::Instruction::ZExt) {
        result = z3::zext(source.bits, after - before);
        if (instruction.hasNonNeg()) valid = valid && source.bits.extract(before - 1, before - 1) == 0;
    } else if (instruction.getOpcode() == llvm::Instruction::SExt) {
        result = z3::sext(source.bits, after - before);
    } else throw std::invalid_argument("unsupported integer cast");
    return {result, valid, source.known, source.unmodeledDependencies};
}

Value compare(const llvm::Instruction& instruction, const Value& left, const Value& right) {
    const auto& comparison = llvm::cast<llvm::ICmpInst>(instruction);
    const auto& a = left.bits;
    const auto& b = right.bits;
    auto condition = a == b;
    switch (comparison.getPredicate()) {
    case llvm::CmpInst::ICMP_EQ: condition = a == b; break;
    case llvm::CmpInst::ICMP_NE: condition = a != b; break;
    case llvm::CmpInst::ICMP_SLT: condition = a < b; break;
    case llvm::CmpInst::ICMP_SLE: condition = a <= b; break;
    case llvm::CmpInst::ICMP_SGT: condition = a > b; break;
    case llvm::CmpInst::ICMP_SGE: condition = a >= b; break;
    case llvm::CmpInst::ICMP_ULT: condition = z3::ult(a, b); break;
    case llvm::CmpInst::ICMP_ULE: condition = z3::ule(a, b); break;
    case llvm::CmpInst::ICMP_UGT: condition = z3::ugt(a, b); break;
    case llvm::CmpInst::ICMP_UGE: condition = z3::uge(a, b); break;
    default: throw std::invalid_argument("unsupported comparison");
    }
    auto valid = left.valid && right.valid;
    if (comparison.hasSameSign()) {
        const auto high = a.get_sort().bv_size() - 1;
        valid = valid && a.extract(high, high) == b.extract(high, high);
    }
    return {booleanBits(condition), valid, left.known && right.known, mergeDependencies(left, right)};
}

Value select(const Value& condition, const Value& yes, const Value& no) {
    const auto test = condition.bits == 1;
    auto dependencies = mergeDependencies(condition, yes);
    dependencies.insert(no.unmodeledDependencies.begin(), no.unmodeledDependencies.end());
    return {z3::ite(test, yes.bits, no.bits), condition.valid && z3::ite(test, yes.valid, no.valid),
            condition.known && z3::ite(test, yes.known, no.known), std::move(dependencies)};
}

z3::expr predicate(const z3::expr& bits, const std::string& comparison, const std::string& text) {
    if (text.empty()) throw std::invalid_argument("--value is required");
    const bool negative = text.front() == '-';
    std::string digits = negative ? text.substr(1) : text;
    const bool hexadecimal = digits.starts_with("0x") || digits.starts_with("0X");
    if (hexadecimal) digits.erase(0, 2);
    if (digits.empty() || !std::all_of(digits.begin(), digits.end(), [&](unsigned char ch) {
        return hexadecimal ? std::isxdigit(ch) != 0 : std::isdigit(ch) != 0;
    })) throw std::invalid_argument("target value must be a decimal or hexadecimal integer");
    const auto width = bits.get_sort().bv_size();
    if (digits.size() > 1024) throw std::invalid_argument("target value is too long");
    llvm::APInt magnitude(std::max(width + 1, static_cast<unsigned>(digits.size() * 4 + 2)), digits, hexadecimal ? 16 : 10);
    const bool signedComparison = comparison.starts_with('s');
    if (negative && comparison.starts_with('u')) throw std::invalid_argument("negative target value requires a signed comparison or eq/ne");
    const unsigned allowed = signedComparison ? width - 1 : width;
    if ((!negative && magnitude.getActiveBits() > allowed) ||
        (negative && (magnitude.getActiveBits() > width ||
                      magnitude.ugt(llvm::APInt::getOneBitSet(magnitude.getBitWidth(), width - 1))))) {
        throw std::invalid_argument("target value does not fit the requested bit width and comparison interpretation");
    }
    magnitude = magnitude.zextOrTrunc(width);
    if (negative) magnitude.negate();
    llvm::SmallString<64> decimal;
    magnitude.toString(decimal, 10, false);
    const auto value = bits.ctx().bv_val(decimal.c_str(), width);
    if (comparison == "eq") return bits == value;
    if (comparison == "ne") return bits != value;
    if (comparison == "ult") return z3::ult(bits, value);
    if (comparison == "ule") return z3::ule(bits, value);
    if (comparison == "ugt") return z3::ugt(bits, value);
    if (comparison == "uge") return z3::uge(bits, value);
    if (comparison == "slt") return bits < value;
    if (comparison == "sle") return bits <= value;
    if (comparison == "sgt") return bits > value;
    if (comparison == "sge") return bits >= value;
    throw std::invalid_argument("unknown target comparison");
}

} // namespace valuetrace::symbolic
