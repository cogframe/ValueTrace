#pragma once

#include <llvm/IR/Instruction.h>
#include <z3++.h>
#include <vector>
#include <set>
#include <string>

namespace valuetrace::symbolic {

struct Value {
    z3::expr bits;
    z3::expr valid;
    z3::expr known;
    std::set<std::string> unmodeledDependencies;
};

inline std::set<std::string> mergeDependencies(const Value& first, const Value& second) {
    auto result = first.unmodeledDependencies;
    result.insert(second.unmodeledDependencies.begin(), second.unmodeledDependencies.end());
    return result;
}

Value binary(const llvm::Instruction& instruction, const Value& left, const Value& right,
             bool ignoreNoWrap = false);
Value cast(const llvm::Instruction& instruction, const Value& source, bool ignoreNoWrap = false);
Value compare(const llvm::Instruction& instruction, const Value& left, const Value& right);
Value select(const Value& condition, const Value& yes, const Value& no);
// Checks mathematical overflow from the input operands in twice the bit width.
// The target instruction's poison/valid condition must not be assumed here.
z3::expr overflows(const llvm::Instruction& instruction, const z3::expr& left,
                   const z3::expr& right, bool isSigned);
z3::expr predicate(const z3::expr& bits, const std::string& comparison, const std::string& value);
std::string instructionText(const llvm::Instruction& instruction);

} // namespace valuetrace::symbolic
