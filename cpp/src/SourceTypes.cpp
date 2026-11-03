#include "SourceTypes.h"

#include <llvm/BinaryFormat/Dwarf.h>
#include <llvm/IR/DebugProgramInstruction.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Instructions.h>

#include <set>

namespace valuetrace {
namespace {

struct Decoded {
    SourceIntegerKind kind = SourceIntegerKind::Unknown;
    std::string chain;
    std::string reason;
};

Decoded decode(const llvm::DIType* type, unsigned depth = 0) {
    if (!type || depth > 16) return {SourceIntegerKind::Unknown, {}, "missing or cyclic debug type"};
    if (const auto* basic = llvm::dyn_cast<llvm::DIBasicType>(type)) {
        const auto name = basic->getName().str();
        switch (basic->getEncoding()) {
        case llvm::dwarf::DW_ATE_signed:
            return {SourceIntegerKind::Signed, name + " [DW_ATE_signed]", {}};
        case llvm::dwarf::DW_ATE_signed_char:
            return {SourceIntegerKind::Signed, name + " [DW_ATE_signed_char]", {}};
        case llvm::dwarf::DW_ATE_unsigned:
            return {SourceIntegerKind::Unsigned, name + " [DW_ATE_unsigned]", {}};
        case llvm::dwarf::DW_ATE_unsigned_char:
            return {SourceIntegerKind::Unsigned, name + " [DW_ATE_unsigned_char]", {}};
        case llvm::dwarf::DW_ATE_boolean:
            return {SourceIntegerKind::Boolean, name + " [DW_ATE_boolean]", {}};
        default:
            return {SourceIntegerKind::Unknown, name, "unsupported DWARF basic-type encoding"};
        }
    }
    if (const auto* derived = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
        switch (derived->getTag()) {
        case llvm::dwarf::DW_TAG_typedef:
        case llvm::dwarf::DW_TAG_const_type:
        case llvm::dwarf::DW_TAG_volatile_type:
        case llvm::dwarf::DW_TAG_restrict_type: {
            auto base = decode(derived->getBaseType(), depth + 1);
            const auto wrapper = derived->getTag() == llvm::dwarf::DW_TAG_typedef ?
                derived->getName().str() :
                derived->getTag() == llvm::dwarf::DW_TAG_const_type ? "const" :
                derived->getTag() == llvm::dwarf::DW_TAG_volatile_type ? "volatile" : "restrict";
            base.chain = wrapper + " -> " + base.chain;
            return base;
        }
        case llvm::dwarf::DW_TAG_member:
            return {SourceIntegerKind::Unknown, derived->getName().str(),
                    "member or bit-field requires field-specific debug mapping"};
        default:
            return {SourceIntegerKind::Unknown, derived->getName().str(),
                    "derived type is not a scalar integer wrapper"};
        }
    }
    if (const auto* composite = llvm::dyn_cast<llvm::DICompositeType>(type)) {
        if (composite->getTag() == llvm::dwarf::DW_TAG_enumeration_type)
            return {SourceIntegerKind::Unknown, composite->getName().str(),
                    "enum underlying type is not inferred in this version"};
    }
    return {SourceIntegerKind::Unknown, type->getName().str(), "non-scalar source type"};
}

const char* kindName(SourceIntegerKind kind) {
    switch (kind) {
    case SourceIntegerKind::Signed: return "signed";
    case SourceIntegerKind::Unsigned: return "unsigned";
    case SourceIntegerKind::Boolean: return "boolean";
    case SourceIntegerKind::Unknown: return "unknown";
    }
    return "unknown";
}

void appendConflict(std::string& conflicts, const std::string& detail) {
    if (!conflicts.empty()) conflicts += "; ";
    conflicts += detail;
}

SourceIntegerKind interpretationKind(IntegerInterpretation interpretation) {
    switch (interpretation) {
    case IntegerInterpretation::Signed: return SourceIntegerKind::Signed;
    case IntegerInterpretation::Unsigned: return SourceIntegerKind::Unsigned;
    case IntegerInterpretation::BitPattern: return SourceIntegerKind::Unknown;
    }
    return SourceIntegerKind::Unknown;
}

} // namespace

SourceTypes::SourceTypes(llvm::Function& function) : function_(function) {
    const auto observe = [&](const llvm::DILocalVariable* variable, const llvm::DIExpression* expression,
                             bool directValue, const std::vector<llvm::Value*>& values,
                             const llvm::DILocation* location) {
        if (!variable || !directValue || (location && location->getInlinedAt()) ||
            !function_.getSubprogram() || variable->getScope()->getSubprogram() != function_.getSubprogram()) return;
        if (values.size() != 1 || !values[0] || !values[0]->getType()->isIntegerTy()) return;
        if (!expression || expression->getNumElements() != 0) {
            rejected_[values[0]] = "fragment or complex dbg.value expression";
            return;
        }
        direct_[values[0]].push_back({variable->getType(), variable->getName().str()});
    };
    for (auto& block : function_) for (auto& instruction : block) {
        for (auto& record : instruction.getDbgRecordRange()) {
            if (const auto* variable = llvm::dyn_cast<llvm::DbgVariableRecord>(&record)) {
                std::vector<llvm::Value*> values;
                for (auto* value : variable->location_ops()) values.push_back(value);
                observe(variable->getVariable(), variable->getExpression(), variable->isDbgValue(),
                        values, variable->getDebugLoc().get());
            }
        }
        if (const auto* variable = llvm::dyn_cast<llvm::DbgVariableIntrinsic>(&instruction)) {
            std::vector<llvm::Value*> values;
            for (auto* value : variable->location_ops()) values.push_back(value);
            observe(variable->getVariable(), variable->getExpression(), llvm::isa<llvm::DbgValueInst>(variable),
                    values, instruction.getDebugLoc().get());
        }
    }
}

IntegerTypeEvidence SourceTypes::forValue(const llvm::Value& value, const OperationSemantics* operation) const {
    IntegerTypeEvidence result;
    std::string sourceBasis;
    if (const auto found = direct_.find(&value); found != direct_.end()) {
        std::set<SourceIntegerKind> kinds;
        bool incomplete = false;
        for (const auto& observation : found->second) {
            const auto decoded = decode(observation.type);
            if (decoded.kind == SourceIntegerKind::Unknown) {
                result.basis = "type unknown: " + decoded.reason + " (" + observation.variable + ")";
                incomplete = true;
                continue;
            }
            if (observation.type && observation.type->getSizeInBits() &&
                observation.type->getSizeInBits() != value.getType()->getIntegerBitWidth()) {
                result.basis = "type unknown: debug type width differs from LLVM value width";
                incomplete = true;
                continue;
            }
            kinds.insert(decoded.kind);
            if (result.sourceType.empty()) result.sourceType = decoded.chain;
            if (sourceBasis.empty()) sourceBasis = "debug-info dbg.value(" + observation.variable + "): " + decoded.chain;
        }
        if (kinds.size() == 1 && !incomplete) result.sourceKind = *kinds.begin();
        else if (kinds.size() > 1) appendConflict(result.conflict, "conflicting debug-value source types for one LLVM value");
        else if (!kinds.empty() && incomplete)
            appendConflict(result.conflict, "some debug-value mappings for this LLVM value have unknown types");
    } else if (const auto rejected = rejected_.find(&value); rejected != rejected_.end()) {
        result.basis = "type unknown: " + rejected->second;
    } else if (!function_.getSubprogram()) {
        result.basis = "type unknown: debug info missing";
    } else {
        result.basis = "type unknown: no direct dbg.value mapping";
    }

    if (const auto* instruction = llvm::dyn_cast<llvm::Instruction>(&value); operation &&
        (operation->opcode == "add" || operation->opcode == "sub" || operation->opcode == "mul" ||
         operation->opcode == "shl" || operation->opcode == "sdiv" || operation->opcode == "udiv" ||
         operation->opcode == "srem" || operation->opcode == "urem")) {
        std::set<SourceIntegerKind> operandKinds;
        std::string operandBasis;
        bool complete = true;
        const auto count = operation->opcode == "shl" ? 1U : instruction->getNumOperands();
        for (unsigned index = 0; index < count; ++index) {
            const auto* operand = instruction->getOperand(index);
            if (llvm::isa<llvm::ConstantInt>(operand)) continue;
            const auto found = direct_.find(operand);
            if (found == direct_.end()) { complete = false; break; }
            std::set<SourceIntegerKind> current;
            for (const auto& observation : found->second) {
                const auto decoded = decode(observation.type);
                if (decoded.kind == SourceIntegerKind::Unknown ||
                    (observation.type->getSizeInBits() &&
                     observation.type->getSizeInBits() != operand->getType()->getIntegerBitWidth())) {
                    complete = false;
                    break;
                }
                current.insert(decoded.kind);
                if (operandBasis.empty()) operandBasis = "debug-info operand dbg.value(" + observation.variable + "): " + decoded.chain;
            }
            if (current.size() != 1) { complete = false; break; }
            operandKinds.insert(*current.begin());
        }
        if (complete && operandKinds.size() > 1)
            appendConflict(result.conflict, "operand debug source types disagree");
        else if (complete && operandKinds.size() == 1) {
            const auto operandKind = *operandKinds.begin();
            if (result.sourceKind == SourceIntegerKind::Unknown && !direct_.contains(&value)) {
                result.sourceKind = operandKind;
                sourceBasis = operandBasis;
                result.sourceType = operandBasis;
            } else if (result.sourceKind != SourceIntegerKind::Unknown && result.sourceKind != operandKind) {
                appendConflict(result.conflict, "result debug source type " + std::string(kindName(result.sourceKind)) +
                    " conflicts with operand source type " + kindName(operandKind));
            }
        }
    }

    SourceIntegerKind attribute = SourceIntegerKind::Unknown;
    std::string attributeBasis;
    const auto inspectArgument = [&](const llvm::Argument& argument) {
        const auto attrs = function_.getAttributes().getParamAttrs(argument.getArgNo());
        const bool sign = attrs.hasAttribute(llvm::Attribute::SExt);
        const bool zero = attrs.hasAttribute(llvm::Attribute::ZExt);
        if (sign == zero) return;
        const auto kind = sign ? SourceIntegerKind::Signed : SourceIntegerKind::Unsigned;
        const auto basis = "parameter arg" + std::to_string(argument.getArgNo()) + (sign ? " signext" : " zeroext");
        if (attribute != SourceIntegerKind::Unknown && attribute != kind)
            appendConflict(result.conflict, "conflicting parameter extension attributes");
        else { attribute = kind; attributeBasis = basis; }
    };
    if (const auto* argument = llvm::dyn_cast<llvm::Argument>(&value)) inspectArgument(*argument);
    else if (const auto* instruction = llvm::dyn_cast<llvm::Instruction>(&value)) {
        for (const auto& operand : instruction->operands()) {
            if (const auto* argument = llvm::dyn_cast<llvm::Argument>(operand.get());
                argument && argument->getType()->isIntegerTy() &&
                argument->getType()->getIntegerBitWidth() == value.getType()->getIntegerBitWidth()) inspectArgument(*argument);
        }
    }

    const auto instructionKind = operation ? interpretationKind(operation->interpretation) : SourceIntegerKind::Unknown;
    if (result.sourceKind != SourceIntegerKind::Unknown) {
        result.kind = result.sourceKind;
        result.sourceOrAttribute = true;
        result.basis = sourceBasis;
    } else if (attribute != SourceIntegerKind::Unknown) {
        result.kind = attribute;
        result.sourceOrAttribute = true;
        result.basis = "ABI extension attribute: " + attributeBasis;
    } else if (instructionKind != SourceIntegerKind::Unknown) {
        result.kind = instructionKind;
        result.basis = "instruction semantics: " + operation->opcode +
            (operation->predicate.empty() ? "" : " " + operation->predicate);
    }
    if ((result.sourceKind == SourceIntegerKind::Signed || result.sourceKind == SourceIntegerKind::Unsigned) &&
        attribute != SourceIntegerKind::Unknown && result.sourceKind != attribute)
        appendConflict(result.conflict, "debug source type " + std::string(kindName(result.sourceKind)) +
                          " conflicts with " + attributeBasis);
    if ((result.sourceKind == SourceIntegerKind::Signed || result.sourceKind == SourceIntegerKind::Unsigned) &&
        instructionKind != SourceIntegerKind::Unknown &&
        result.sourceKind != instructionKind && (!operation || operation->opcode != "icmp"))
        appendConflict(result.conflict, "debug source type " + std::string(kindName(result.sourceKind)) +
                          " conflicts with instruction " + operation->opcode + " semantics " + kindName(instructionKind));
    if (result.sourceKind == SourceIntegerKind::Unknown && attribute != SourceIntegerKind::Unknown &&
        instructionKind != SourceIntegerKind::Unknown && attribute != instructionKind)
        appendConflict(result.conflict, attributeBasis + " conflicts with instruction " + operation->opcode +
                          " semantics " + kindName(instructionKind));
    return result;
}

} // namespace valuetrace
