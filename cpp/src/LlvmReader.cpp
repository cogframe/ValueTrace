#include "valuetrace/LlvmReader.h"
#include "LlvmSupport.h"
#include "SourceTypes.h"

#include <llvm/ADT/APInt.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Operator.h>
#include <llvm/IR/Operator.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>

namespace valuetrace {
namespace {

unsigned width(const llvm::Value& value) {
    if (value.getType()->isIntegerTy()) return value.getType()->getIntegerBitWidth();
    return value.getType()->isPointerTy() ? 64 : 0;
}

std::string operandText(const llvm::Value& value) {
    std::string text;
    llvm::raw_string_ostream stream(text);
    value.printAsOperand(stream, false);
    return text;
}

llvm::Function* directCallee(const llvm::CallBase& call) {
    return llvm::dyn_cast<llvm::Function>(call.getCalledOperand()->stripPointerCasts());
}

SourceLocation location(const llvm::Instruction& instruction) {
    const auto& debug = instruction.getDebugLoc();
    if (!debug) return {};
    auto file = std::filesystem::path(debug->getFilename().str());
    if (file.is_relative()) file = std::filesystem::path(debug->getDirectory().str()) / file;
    return {file.generic_string(), debug.getLine(), debug.getCol()};
}

OperationSemantics semantics(const llvm::Instruction& instruction) {
    OperationSemantics result;
    result.opcode = instruction.getOpcodeName();
    llvm::raw_string_ostream stream(result.ir);
    instruction.print(stream);
    if (const auto* op = llvm::dyn_cast<llvm::OverflowingBinaryOperator>(&instruction)) {
        result.noSignedWrap = op->hasNoSignedWrap();
        result.noUnsignedWrap = op->hasNoUnsignedWrap();
    }
    if (const auto* op = llvm::dyn_cast<llvm::PossiblyExactOperator>(&instruction)) {
        result.exact = op->isExact();
    }
    if (const auto* op = llvm::dyn_cast<llvm::TruncInst>(&instruction)) {
        result.noSignedWrap = op->hasNoSignedWrap();
        result.noUnsignedWrap = op->hasNoUnsignedWrap();
    }
    if (llvm::isa<llvm::PossiblyNonNegInst>(instruction)) result.nonNegative = instruction.hasNonNeg();
    if (const auto* op = llvm::dyn_cast<llvm::PossiblyDisjointInst>(&instruction)) result.disjoint = op->isDisjoint();
    switch (instruction.getOpcode()) {
    case llvm::Instruction::SDiv: case llvm::Instruction::SRem:
    case llvm::Instruction::AShr: case llvm::Instruction::SExt:
        result.interpretation = IntegerInterpretation::Signed;
        break;
    case llvm::Instruction::UDiv: case llvm::Instruction::URem:
    case llvm::Instruction::LShr: case llvm::Instruction::ZExt:
        result.interpretation = IntegerInterpretation::Unsigned;
        break;
    default: break;
    }
    if (const auto* compare = llvm::dyn_cast<llvm::ICmpInst>(&instruction)) {
        result.predicate = llvm::CmpInst::getPredicateName(compare->getPredicate()).str();
        result.sameSign = compare->hasSameSign();
        if (compare->isSigned()) result.interpretation = IntegerInterpretation::Signed;
        else if (compare->isUnsigned()) result.interpretation = IntegerInterpretation::Unsigned;
    }
    return result;
}

struct AddBounds {
    llvm::APInt signedMinimum;
    llvm::APInt signedMaximum;
    llvm::APInt unsignedMaximum;
};

std::optional<AddBounds> addOperandBounds(const llvm::Value& value, unsigned wide) {
    if (const auto* constant = llvm::dyn_cast<llvm::ConstantInt>(&value)) {
        const auto& bits = constant->getValue();
        return AddBounds{bits.sext(wide), bits.sext(wide), bits.zext(wide)};
    }
    if (const auto* extension = llvm::dyn_cast<llvm::ZExtInst>(&value)) {
        const auto bits = extension->getSrcTy()->getIntegerBitWidth();
        const auto maximum = llvm::APInt::getMaxValue(bits).zext(wide);
        return AddBounds{llvm::APInt(wide, 0), maximum, maximum};
    }
    if (const auto* extension = llvm::dyn_cast<llvm::SExtInst>(&value)) {
        const auto bits = extension->getSrcTy()->getIntegerBitWidth();
        return AddBounds{llvm::APInt::getSignedMinValue(bits).sext(wide),
                         llvm::APInt::getSignedMaxValue(bits).sext(wide),
                         llvm::APInt::getMaxValue(value.getType()->getIntegerBitWidth()).zext(wide)};
    }
    return std::nullopt;
}

std::pair<bool, bool> possibleAddOverflows(const llvm::Instruction& instruction) {
    const auto width = instruction.getType()->getIntegerBitWidth();
    const auto wide = width * 2;
    const auto left = addOperandBounds(*instruction.getOperand(0), wide);
    const auto right = addOperandBounds(*instruction.getOperand(1), wide);
    if (!left || !right) return {true, true};
    const auto signedMinimum = llvm::APInt::getSignedMinValue(width).sext(wide);
    const auto signedMaximum = llvm::APInt::getSignedMaxValue(width).sext(wide);
    const auto unsignedMaximum = llvm::APInt::getMaxValue(width).zext(wide);
    const bool signedRisk = (left->signedMinimum + right->signedMinimum).slt(signedMinimum) ||
                            (left->signedMaximum + right->signedMaximum).sgt(signedMaximum);
    const bool unsignedRisk = (left->unsignedMaximum + right->unsignedMaximum).ugt(unsignedMaximum);
    return {signedRisk, unsignedRisk};
}

// A warning means that some operand bit patterns could trigger the condition.
// Branch feasibility and source-level signedness are intentionally not inferred
// here; the CLI reports these candidates as unverified.
std::vector<ArithmeticRisk> arithmeticRisks(const llvm::Instruction& instruction) {
    std::vector<ArithmeticRisk> risks;
    const auto opcode = instruction.getOpcode();
    if (instruction.getNumOperands() == 0) return risks;
    const auto* left = llvm::dyn_cast<llvm::ConstantInt>(instruction.getOperand(0));
    const auto* right = instruction.getNumOperands() >= 2 ? llvm::dyn_cast<llvm::ConstantInt>(instruction.getOperand(1)) : nullptr;
    const auto isZero = [](const llvm::ConstantInt* value) { return value && value->isZero(); };
    const auto isOne = [](const llvm::ConstantInt* value) { return value && value->isOne(); };
    const auto add = [&](ArithmeticRiskReason reason, IntegerInterpretation assumption) {
        risks.push_back({reason, assumption});
    };

    if (opcode == llvm::Instruction::Trunc) {
        const auto after = instruction.getType()->getIntegerBitWidth();
        if (!left || left->getValue().trunc(after).sext(left->getBitWidth()) != left->getValue())
            add(ArithmeticRiskReason::SignedTruncationLoss, IntegerInterpretation::Signed);
        if (!left || left->getValue().trunc(after).zext(left->getBitWidth()) != left->getValue())
            add(ArithmeticRiskReason::UnsignedTruncationLoss, IntegerInterpretation::Unsigned);
    } else if (opcode == llvm::Instruction::Shl || opcode == llvm::Instruction::LShr || opcode == llvm::Instruction::AShr) {
        const auto bits = instruction.getType()->getIntegerBitWidth();
        const bool invalidConstant = right && right->getValue().uge(bits);
        if (!right || invalidConstant) add(ArithmeticRiskReason::InvalidShiftAmount, IntegerInterpretation::BitPattern);
        // Value overflow is a separate check, only for an otherwise legal amount.
        if (opcode == llvm::Instruction::Shl && !invalidConstant && !isZero(right) && !isZero(left)) {
            bool signedRisk = true, unsignedRisk = true;
            if (left && right) {
                static_cast<void>(left->getValue().sshl_ov(right->getValue(), signedRisk));
                static_cast<void>(left->getValue().ushl_ov(right->getValue(), unsignedRisk));
            }
            if (signedRisk) add(ArithmeticRiskReason::SignedOverflow, IntegerInterpretation::Signed);
            if (unsignedRisk) add(ArithmeticRiskReason::UnsignedWrap, IntegerInterpretation::Unsigned);
        }
    } else if (opcode == llvm::Instruction::Add || opcode == llvm::Instruction::Sub ||
        opcode == llvm::Instruction::Mul) {
        bool signedRisk = true, unsignedRisk = true;
        if (left && right) {
            bool overflow = false;
            if (opcode == llvm::Instruction::Add) static_cast<void>(left->getValue().sadd_ov(right->getValue(), overflow));
            else if (opcode == llvm::Instruction::Sub) static_cast<void>(left->getValue().ssub_ov(right->getValue(), overflow));
            else static_cast<void>(left->getValue().smul_ov(right->getValue(), overflow));
            signedRisk = overflow;
            if (opcode == llvm::Instruction::Add) static_cast<void>(left->getValue().uadd_ov(right->getValue(), overflow));
            else if (opcode == llvm::Instruction::Sub) static_cast<void>(left->getValue().usub_ov(right->getValue(), overflow));
            else static_cast<void>(left->getValue().umul_ov(right->getValue(), overflow));
            unsignedRisk = overflow;
        } else if (opcode == llvm::Instruction::Add) {
            signedRisk = unsignedRisk = !isZero(left) && !isZero(right);
        } else if (opcode == llvm::Instruction::Sub) {
            const bool sameOperand = instruction.getOperand(0) == instruction.getOperand(1);
            signedRisk = unsignedRisk = !sameOperand && !isZero(right);
        } else {
            const bool harmless = isZero(left) || isZero(right) || isOne(left) || isOne(right);
            signedRisk = unsignedRisk = !harmless;
        }
        if (opcode == llvm::Instruction::Add) {
            const auto [possibleSigned, possibleUnsigned] = possibleAddOverflows(instruction);
            signedRisk &= possibleSigned;
            unsignedRisk &= possibleUnsigned;
        }
        if (signedRisk) add(ArithmeticRiskReason::SignedOverflow, IntegerInterpretation::Signed);
        if (unsignedRisk) {
            add(opcode == llvm::Instruction::Sub ? ArithmeticRiskReason::UnsignedUnderflow
                                                : ArithmeticRiskReason::UnsignedWrap,
                IntegerInterpretation::Unsigned);
        }
    } else if (opcode == llvm::Instruction::SDiv || opcode == llvm::Instruction::UDiv ||
               opcode == llvm::Instruction::SRem || opcode == llvm::Instruction::URem) {
        if (!right || right->isZero()) add(ArithmeticRiskReason::DivideByZero, IntegerInterpretation::BitPattern);
        if ((opcode == llvm::Instruction::SDiv || opcode == llvm::Instruction::SRem) &&
            (!left || left->getValue().isMinSignedValue()) &&
            (!right || right->getValue().isAllOnes())) {
            add(ArithmeticRiskReason::SignedDivisionOverflow, IntegerInterpretation::Signed);
        }
    }
    return risks;
}

void applyTypeEvidence(std::vector<ArithmeticRisk>& risks, const IntegerTypeEvidence& type,
                       const OperationSemantics& operation) {
    if (!type.sourceOrAttribute || !type.conflict.empty()) return;
    std::erase_if(risks, [&](const ArithmeticRisk& risk) {
        if (type.kind == SourceIntegerKind::Signed && risk.assumption == IntegerInterpretation::Unsigned)
            return !operation.noUnsignedWrap;
        if (type.kind == SourceIntegerKind::Unsigned && risk.assumption == IntegerInterpretation::Signed)
            return !operation.noSignedWrap;
        return false;
    });
}

using selection::functionName;

struct Frame {
    llvm::Function* function;
    std::string context;
    std::map<const llvm::Value*, NodeId> values;
    std::vector<std::optional<NodeId>> parameters;
    std::vector<NodeId> returns;
    std::string rootFunction;
    std::vector<CallSiteIdentity> callPath;
    Frame* parent = nullptr;
    const llvm::CallBase* incomingCall = nullptr;
};

// Expand a finite call tree, with distinct value identities for every call
// context. Reverse call paths choose contexts containing the selected function.
class Importer {
public:
    Importer(llvm::Module& module, const ParameterQuery& query)
        : module_(module), request_(query), selected_(selection::resolveFunction(module, query.function)) {
        if (query.upDepth > 64 || query.downDepth > 64) {
            throw std::invalid_argument("import depth must not exceed 64 in this version");
        }
        if (query.parameters.empty() && !query.selectReturn) throw std::invalid_argument("select at least one parameter");
        if (query.selectReturn && !selected_.getReturnType()->isIntegerTy())
            throw std::invalid_argument("--return-of requires a scalar integer return type");
        for (auto index : query.parameters) {
            if (index >= selected_.arg_size()) throw std::invalid_argument("parameter index out of range");
            if (!selected_.getArg(index)->getType()->isIntegerTy() && !selected_.getArg(index)->getType()->isPointerTy()) {
                throw std::invalid_argument("selected parameter must have a scalar integer or pointer LLVM type");
            }
        }
        for (auto& function : module_) {
            unsigned index = 0;
            for (auto& block : function) {
                for (auto& instruction : block) {
                    ordinals_[&instruction] = index;
                    if (auto* call = llvm::dyn_cast<llvm::CallBase>(&instruction)) {
                        sites_[call] = function.getName().str() + ":call" + std::to_string(index);
                        if (auto* callee = directCallee(*call)) callers_[callee].push_back(call);
                    }
                    ++index;
                }
            }
        }
    }

    ImportedProgram run() {
        std::vector<llvm::CallBase*> reversePath;
        enumerateCallers(selected_, request_.upDepth + 1, reversePath);
        const auto expansionDepth = request_.upDepth + std::max(request_.upDepth, request_.downDepth) + 2;
        std::set<NodeId> seeds;
        for (const auto& path : paths_) {
            auto* root = path.empty() ? &selected_ : path.front()->getFunction();
            auto context = "root:" + root->getName().str();
            createFrame(*root, context, expansionDepth, root->getName().str(), {});
            for (auto* call : path) context += "/" + sites_.at(call);
            const auto& selectedFrame = *frames_.at(context);
            if (request_.selectReturn) {
                for (auto id : selectedFrame.returns) seeds.insert(id);
            } else for (auto index : request_.parameters) seeds.insert(*selectedFrame.parameters.at(index));
        }
        for (const auto& [key, written] : memoryStores_)
            for (const auto& [readKey, loaded] : memoryLoads_)
                if (key == readKey) {
                    const auto& source = result_.graph.node(written);
                    const auto& target = result_.graph.node(loaded);
                    const bool local = source.function == target.function && source.context == target.context;
                    result_.graph.addEdge(written, loaded,
                        local ? EdgeKind::Local : EdgeKind::CallBoundary,
                        local ? "" : "memory:" + key);
                }
        result_.query = {{seeds.begin(), seeds.end()}, request_.upDepth, request_.downDepth};
        result_.diagnostics.assign(diagnostics_.begin(), diagnostics_.end());
        return std::move(result_);
    }

private:
    std::optional<std::pair<std::string, std::int64_t>> memoryLocation(
            const Frame& frame, const llvm::Value& value) const {
        if (const auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(&value))
            return std::pair{frame.context + ":" + operandText(*alloca), std::int64_t{0}};
        if (const auto* arg = llvm::dyn_cast<llvm::Argument>(&value)) {
            if (frame.parent && frame.incomingCall && arg->getArgNo() < frame.incomingCall->arg_size())
                return memoryLocation(*frame.parent, *frame.incomingCall->getArgOperand(arg->getArgNo()));
            return std::pair{frame.context + ":arg" + std::to_string(arg->getArgNo()), std::int64_t{0}};
        }
        if (const auto* cast = llvm::dyn_cast<llvm::BitCastOperator>(&value))
            return memoryLocation(frame, *cast->getOperand(0));
        if (const auto* gep = llvm::dyn_cast<llvm::GEPOperator>(&value)) {
            auto base = memoryLocation(frame, *gep->getPointerOperand());
            if (!base) return std::nullopt;
            llvm::APInt offset(module_.getDataLayout().getPointerSizeInBits(), 0);
            if (!gep->accumulateConstantOffset(module_.getDataLayout(), offset)) return std::nullopt;
            base->second += offset.getSExtValue();
            return base;
        }
        return std::nullopt;
    }

    std::optional<std::string> memoryKey(const Frame& frame, const llvm::Value& pointer,
                                          unsigned bits) const {
        auto at = memoryLocation(frame, pointer);
        if (!at) return std::nullopt;
        return at->first + ":" + std::to_string(at->second) + ":" + std::to_string((bits + 7) / 8);
    }
    void enumerateCallers(llvm::Function& function, unsigned remaining,
                          std::vector<llvm::CallBase*>& reversePath) {
        const auto found = callers_.find(&function);
        if (remaining == 0 || found == callers_.end() || found->second.empty()) {
            if (paths_.size() >= request_.maxContexts) throw std::runtime_error("caller context budget exceeded");
            paths_.emplace_back(reversePath.rbegin(), reversePath.rend());
            return;
        }
        for (auto* call : found->second) {
            reversePath.push_back(call);
            enumerateCallers(*call->getFunction(), remaining - 1, reversePath);
            reversePath.pop_back();
        }
    }

    NodeId add(Node node) {
        if (result_.graph.size() >= request_.maxNodes) throw std::runtime_error("import node budget exceeded");
        return result_.graph.addNode(std::move(node));
    }

    Node makeNode(const Frame& frame, std::string label, unsigned bits, NodeKind kind) {
        Node result{frame.function->getName().str(), std::move(label), bits, kind, frame.context};
        result.displayFunction = functionName(*frame.function);
        result.rootFunction = frame.rootFunction;
        result.callPath = frame.callPath;
        return result;
    }

    NodeId valueNode(Frame& frame, const llvm::Value& value) {
        if (auto found = frame.values.find(&value); found != frame.values.end()) return found->second;
        auto node = makeNode(frame, operandText(value), width(value), NodeKind::Constant);
        if (!llvm::isa<llvm::ConstantInt>(value)) {
            node.unresolved = "non-concrete integer value (undef, poison, or unsupported producer)";
        }
        const auto id = add(std::move(node));
        frame.values[&value] = id;
        return id;
    }

    Frame& createFrame(llvm::Function& function, const std::string& context, unsigned remaining,
                       const std::string& rootFunction, const std::vector<CallSiteIdentity>& callPath,
                       Frame* parent = nullptr, const llvm::CallBase* incoming = nullptr) {
        if (auto found = frames_.find(context); found != frames_.end()) return *found->second;
        if (frames_.size() >= request_.maxContexts) throw std::runtime_error("call context budget exceeded");
        auto storage = std::make_unique<Frame>();
        auto& frame = *storage;
        frame.function = &function;
        frame.context = context;
        frame.rootFunction = rootFunction;
        frame.callPath = callPath;
        frame.parent = parent;
        frame.incomingCall = incoming;
        frames_.emplace(context, std::move(storage));
        SourceTypes sourceTypes(function);

        const bool frontier = function.isDeclaration() || remaining == 0;
        const std::string reason = function.isDeclaration() ? "external function definition unavailable" : "import expansion limit";
        for (auto& argument : function.args()) {
            if (!width(argument)) {
                frame.parameters.push_back(std::nullopt);
                continue;
            }
            auto node = makeNode(frame, "arg" + std::to_string(argument.getArgNo()) + " " + operandText(argument),
                                 width(argument), NodeKind::Parameter);
            node.integerType = sourceTypes.forValue(argument);
            if (frontier) node.unresolved = reason;
            const auto id = add(std::move(node));
            frame.parameters.push_back(id);
            frame.values[&argument] = id;
        }
        if (frontier) {
            if (function.getReturnType()->isIntegerTy()) {
                auto node = makeNode(frame, "unresolved return", function.getReturnType()->getIntegerBitWidth(), NodeKind::ReturnValue);
                node.unresolved = reason;
                frame.returns.push_back(add(std::move(node)));
            }
            diagnostics_.insert(function.getName().str() + ": " + reason);
            return frame;
        }
        if (function.size() > 1) diagnostics_.insert(function.getName().str() + ": branch/loop value dependencies only; path feasibility not checked");

        // Allocate first, then connect: PHI and loop references can point forward.
        for (auto& block : function) {
            for (auto& instruction : block) {
                if (!width(instruction)) continue;
                auto node = makeNode(frame, operandText(instruction), width(instruction), NodeKind::Operation);
                node.operation = semantics(instruction);
                node.integerType = sourceTypes.forValue(instruction, &node.operation);
                node.arithmeticRisks = arithmeticRisks(instruction);
                applyTypeEvidence(node.arithmeticRisks, node.integerType, node.operation);
                node.instructionOrdinal = ordinals_.at(&instruction);
                node.source = location(instruction);
                if (const auto* load = llvm::dyn_cast<llvm::LoadInst>(&instruction)) {
                    if (!memoryKey(frame, *load->getPointerOperand(), width(instruction)))
                        node.unresolved = "memory read pointer unresolved";
                }
                else if (llvm::isa<llvm::PtrToIntInst>(instruction)) node.unresolved = "pointer-to-integer dependency not modeled";
                else if (!(instruction.isBinaryOp() || llvm::isa<llvm::CastInst>(instruction) ||
                           llvm::isa<llvm::ICmpInst>(instruction) || llvm::isa<llvm::CallInst>(instruction) ||
                           llvm::isa<llvm::PHINode>(instruction) || llvm::isa<llvm::SelectInst>(instruction))) {
                    node.unresolved = "unsupported integer producer";
                }
                frame.values[&instruction] = add(std::move(node));
            }
        }
        for (auto& block : function) {
            for (auto& instruction : block) {
                if (auto* call = llvm::dyn_cast<llvm::CallBase>(&instruction)) {
                    connectCall(frame, *call, remaining);
                } else if (auto* ret = llvm::dyn_cast<llvm::ReturnInst>(&instruction)) {
                    if (auto* value = ret->getReturnValue(); value && width(*value)) {
                        auto node = makeNode(frame, "return " + operandText(*value), width(*value), NodeKind::ReturnValue);
                        node.operation = semantics(instruction);
                        node.source = location(instruction);
                        const auto id = add(std::move(node));
                        frame.returns.push_back(id);
                        result_.graph.addEdge(valueNode(frame, *value), id, EdgeKind::Local);
                    }
                } else if (width(instruction)) {
                    if (const auto* load = llvm::dyn_cast<llvm::LoadInst>(&instruction))
                        if (auto key = memoryKey(frame, *load->getPointerOperand(), width(instruction)))
                            memoryLoads_.emplace_back(*key, frame.values.at(&instruction));
                    for (unsigned index = 0; index < instruction.getNumOperands(); ++index) {
                        const auto& operand = *instruction.getOperand(index);
                        if (width(operand)) {
                            result_.graph.addEdge(valueNode(frame, operand), frame.values.at(&instruction), EdgeKind::Local, {}, index);
                        }
                    }
                } else if (instruction.mayReadOrWriteMemory()) {
                    if (const auto* store = llvm::dyn_cast<llvm::StoreInst>(&instruction))
                        if (width(*store->getValueOperand()) && store->getValueOperand()->getType()->isIntegerTy())
                            if (auto key = memoryKey(frame, *store->getPointerOperand(), width(*store->getValueOperand())))
                                memoryStores_.emplace_back(*key, valueNode(frame, *store->getValueOperand()));
                    diagnostics_.insert(function.getName().str() + ": memory effects not modeled");
                }
            }
        }
        return frame;
    }

    void connectCall(Frame& caller, llvm::CallBase& call, unsigned remaining) {
        auto* callee = directCallee(call);
        const auto& site = sites_.at(&call);
        auto path = caller.callPath;
        path.push_back({caller.function->getName().str(), ordinals_.at(&call)});
        Frame* child = callee ? &createFrame(*callee, caller.context + "/" + site, remaining - 1, caller.rootFunction, path, &caller, &call) : nullptr;
        for (unsigned index = 0; index < call.arg_size(); ++index) {
            const auto& argument = *call.getArgOperand(index);
            if (!width(argument)) continue;
            auto node = makeNode(caller, "call " + (callee ? callee->getName().str() : "<indirect>") +
                                 " arg" + std::to_string(index), width(argument), NodeKind::Use);
            node.operation.opcode = "call.argument";
            node.source = location(call);
            if (!child || index >= child->parameters.size() || !child->parameters[index]) {
                node.unresolved = "indirect, variadic, or non-integer formal parameter";
            }
            const auto actual = add(std::move(node));
            result_.graph.addEdge(valueNode(caller, argument), actual, EdgeKind::Local, {}, index);
            if (child && index < child->parameters.size() && child->parameters[index]) {
                result_.graph.addEdge(actual, *child->parameters[index], EdgeKind::CallBoundary, site, index);
            }
        }
        if (width(call)) {
            const auto result = caller.values.at(&call);
            if (callee && callee->isIntrinsic() &&
                (callee->getName().starts_with("llvm.abs.") ||
                 callee->getName().starts_with("llvm.smin.") ||
                 callee->getName().starts_with("llvm.smax.") ||
                 callee->getName().starts_with("llvm.umin.") ||
                 callee->getName().starts_with("llvm.umax."))) {
                for (unsigned index = 0; index < call.arg_size(); ++index)
                    if (width(*call.getArgOperand(index)))
                        result_.graph.addEdge(valueNode(caller, *call.getArgOperand(index)), result,
                                              EdgeKind::Local, {}, index);
            }
            if (child) {
                for (auto returned : child->returns) result_.graph.addEdge(returned, result, EdgeKind::CallBoundary, site);
            } else {
                diagnostics_.insert(caller.function->getName().str() + ": indirect call result dependency unresolved at " + site);
            }
        }
        if (!child) diagnostics_.insert(caller.function->getName().str() + ": indirect call unresolved at " + site);
    }

    llvm::Module& module_;
    const ParameterQuery& request_;
    llvm::Function& selected_;
    ImportedProgram result_;
    std::map<const llvm::Function*, std::vector<llvm::CallBase*>> callers_;
    std::map<const llvm::CallBase*, std::string> sites_;
    std::map<const llvm::Instruction*, unsigned> ordinals_;
    std::map<std::string, std::unique_ptr<Frame>> frames_;
    std::vector<std::vector<llvm::CallBase*>> paths_;
    std::set<std::string> diagnostics_;
    std::vector<std::pair<std::string, NodeId>> memoryStores_;
    std::vector<std::pair<std::string, NodeId>> memoryLoads_;
};

} // namespace

ImportedProgram readParameterGraph(const std::filesystem::path& file, const ParameterQuery& query) {
    return readParameterGraph(std::vector<std::filesystem::path>{file}, query);
}
ImportedProgram readParameterGraph(const std::vector<std::filesystem::path>& files, const ParameterQuery& query) {
    llvm::LLVMContext context;
    auto module = selection::loadModule(files, context);
    auto& function = selection::resolveFunction(*module, query.function);
    const auto description = selection::describeFunction(function);
    auto resolved = query;
    std::vector<std::string> selections;
    if (!query.selectReturn)
        resolved.parameters = selection::resolveParameters(description, query.parameters, query.parameterNames, selections);
    else selections.push_back("Function return value: " + functionName(function));
    resolved.function = function.getName().str();
    auto result = Importer(*module, resolved).run();
    result.selections = std::move(selections);
    result.diagnostics.insert(result.diagnostics.end(), description.notes.begin(), description.notes.end());
    return result;
}

} // namespace valuetrace
