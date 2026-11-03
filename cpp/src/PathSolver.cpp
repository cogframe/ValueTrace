#include "valuetrace/PathSolver.h"
#include "IntegerExpressions.h"
#include "LlvmSupport.h"
#include "InstructionRisk.h"
#include "RiskConditions.h"

#include <llvm/ADT/SmallString.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Operator.h>
#include <llvm/Support/raw_ostream.h>

#include <chrono>
#include <algorithm>
#include <deque>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>

namespace valuetrace {
namespace {

using symbolic::Value;
using Arguments = std::vector<std::optional<Value>>;
using Context = std::vector<const llvm::CallBase*>;

struct PointerLocation { std::string object; std::int64_t offset = 0; };
using PointerArguments = std::vector<std::optional<PointerLocation>>;
struct MemorySlot { Value value; std::string lastWrite; };

struct Observed {
    std::string name;
    z3::expr bits;
};

struct Path {
    z3::expr condition;
    z3::expr known;
    std::vector<std::string> trace;
    std::vector<std::string> constraints;
    std::set<std::string> unmodeledDependencies;
    std::set<std::string> possibleMemoryWrites;
    std::vector<Observed> randCalls;
    std::map<std::string, MemorySlot> memory;
    std::set<std::string> escapedObjects;
    std::set<std::string> havocObjects;
    std::map<std::string, std::string> singleLocationByObject;
    std::set<std::string> multiLocationObjects;
    std::set<std::string> memoryEvidence;
};

struct State {
    Path path;
    std::map<const llvm::Value*, Value> values;
    std::map<const llvm::Value*, PointerLocation> pointers;
    std::set<const llvm::BasicBlock*> visited;
    const llvm::Instruction* next = nullptr;
};

struct Returned {
    Path path;
    std::optional<Value> value;
};

std::string operandName(const llvm::Value& value) {
    std::string text;
    llvm::raw_string_ostream stream(text);
    value.printAsOperand(stream, false);
    return text;
}

std::string blockName(const llvm::BasicBlock& block) {
    return selection::functionName(*block.getParent()) + ":" + operandName(block);
}

std::string sourceSuffix(const llvm::Instruction& instruction) {
    const auto& location = instruction.getDebugLoc();
    if (!location) return {};
    return " at " + location->getFilename().str() + ":" + std::to_string(location.getLine());
}

llvm::Function* directCallee(const llvm::CallBase& call) {
    return llvm::dyn_cast<llvm::Function>(call.getCalledOperand()->stripPointerCasts());
}

bool discardedDependency(std::string_view reason) {
    return reason.starts_with("unexplored path:") || reason.starts_with("unsupported value dependency:") ||
           reason.starts_with("unsupported control dependency:") ||
           reason.starts_with("unsupported control transfer:") || reason.starts_with("PHI predecessor unavailable") ||
           reason.starts_with("Z3:") || reason.find("budget") != std::string_view::npos ||
           reason.find("timeout") != std::string_view::npos;
}

bool declaredExternalInput(std::string_view reason, const std::vector<std::string>& sources) {
    constexpr std::string_view prefix = "unmodeled return value from external call before target: ";
    if (!reason.starts_with(prefix)) return false;
    const auto name = reason.substr(prefix.size());
    return std::find(sources.begin(), sources.end(), name) != sources.end();
}

std::string numeral(const z3::expr& value) {
    std::string text;
    if (!value.is_numeral(text)) throw std::runtime_error("solver model did not assign a concrete integer");
    return text;
}

WitnessValue observe(const z3::model& model, const std::string& name, const z3::expr& bits) {
    const auto text = numeral(model.eval(bits, true));
    const auto width = bits.get_sort().bv_size();
    llvm::APInt number(width, text, 10);
    llvm::SmallString<64> signedText;
    number.toString(signedText, 10, true);
    return {name, width, text, signedText.str().str()};
}

// A path is evaluated with actual predecessor and call contexts, rather than
// taking the union of values from incompatible branches or different callers.
class Executor {
public:
    Executor(llvm::Module& module, const ReachabilityQuery& query, const execution::InstructionRiskTarget* risk = nullptr)
        : module_(module), query_(query), target_(selection::resolveFunction(module, query.target.function)), risk_(risk),
          started_(std::chrono::steady_clock::now()) {
        if (query.target.upDepth > 64 || query.target.downDepth != 0) {
            throw std::invalid_argument("solve uses --up 0..64 for a function-entry query; --down is not used by this command");
        }
        if (query.timeoutMs == 0 || query.maxStates == 0 || query.maxSteps == 0) {
            throw std::invalid_argument("solver time/state/step limits must be positive");
        }
        if (query.randMaxOverride && *query.randMaxOverride > 2147483647U)
            throw std::invalid_argument("--rand-max must not exceed INT_MAX for a C int return value");
        if (!query.randUnbounded) {
            if (query.randMaxOverride) {
                randMax_ = *query.randMaxOverride;
                randRangeBasis_ = "range verified against --rand-max=" + std::to_string(*randMax_);
            } else if (module.getTargetTriple().str().find("windows-msvc") != std::string::npos) {
                randMax_ = 32767;
                randRangeBasis_ = "range verified against Windows MSVC RAND_MAX=32767 (target triple)";
            } else if (module.getTargetTriple().str().find("linux-gnu") != std::string::npos) {
                randMax_ = 2147483647;
                randRangeBasis_ = "range verified against Linux GNU RAND_MAX=2147483647 (target triple)";
            }
        }
        if (randRangeBasis_.empty()) randRangeBasis_ = "range unverified; no rand bound for this target triple";
        const auto description = selection::describeFunction(target_);
        if (!risk_) {
            const auto indices = selection::resolveParameters(description, query.target.parameters, query.target.parameterNames, result_.selections);
            if (indices.size() != 1) throw std::invalid_argument("solve checks exactly one target parameter; other parameters remain jointly symbolic");
            parameter_ = indices.front();
            const auto bits = target_.getArg(parameter_)->getType()->getIntegerBitWidth();
            symbolic::predicate(z3_.bv_const("validate_target", bits), query.comparison, query.value);
        }
        notes_.insert(description.notes.begin(), description.notes.end());
        if (risk_ && risk_->closedUpDepth) notes_.insert("Data slice closed at upstream depth " +
            std::to_string(*risk_->closedUpDepth) + ": " + risk_->closureReason);
        for (auto& function : module_) {
            for (auto& block : function) for (auto& instruction : block) {
                if (auto* call = llvm::dyn_cast<llvm::CallBase>(&instruction)) {
                    if (auto* callee = directCallee(*call)) callers_[callee].push_back(call);
                }
            }
        }
    }

    ReachabilityResult run() {
        Context reverse;
        if (risk_) targets_.push_back(risk_->context);
        else enumerate(target_, query_.target.upDepth, reverse);
        unsigned rootIndex = 0;
        for (const auto& path : targets_) {
            if (!budget()) break;
            expected_ = path;
            auto* root = path.empty() ? &target_ : module_.getFunction(path.front()->getFunction()->getName());
            if (risk_) notes_.insert("Verification root: " + selection::functionName(*root) +
                "; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.");
            rootInputs_.clear();
            Arguments arguments;
            PointerArguments pointerArguments;
            const auto description = selection::describeFunction(*root);
            for (auto& argument : root->args()) {
                if (argument.getType()->isPointerTy()) {
                    arguments.push_back(std::nullopt);
                    pointerArguments.push_back(PointerLocation{"root" + std::to_string(rootIndex) + ".arg" +
                        std::to_string(argument.getArgNo()), 0});
                    continue;
                }
                pointerArguments.push_back(std::nullopt);
                if (!argument.getType()->isIntegerTy()) { arguments.push_back(std::nullopt); continue; }
                const auto width = argument.getType()->getIntegerBitWidth();
                const auto symbol = "root" + std::to_string(rootIndex) + ".arg" + std::to_string(argument.getArgNo());
                auto bits = z3_.bv_const(symbol.c_str(), width);
                std::string name = "arg" + std::to_string(argument.getArgNo());
                for (const auto& candidate : description.parameters) {
                    if (candidate.llvmIndex == argument.getArgNo() && !candidate.name.empty()) name = candidate.name;
                }
                rootInputs_.push_back({selection::functionName(*root) + "." + name, bits});
                freeValues_.insert(selection::functionName(*root) + "." + name + " i" + std::to_string(width));
                arguments.push_back(Value{bits, z3_.bool_val(true), z3_.bool_val(true)});
            }
            Path initial{z3_.bool_val(true), z3_.bool_val(true),
                         {"Boundary entry: " + selection::functionName(*root)}, {}};
            if (std::count_if(pointerArguments.begin(), pointerArguments.end(),
                [](const auto& at) { return at.has_value(); }) > 1) {
                for (const auto& at : pointerArguments) if (at) {
                    initial.escapedObjects.insert(at->object);
                    initial.memoryEvidence.insert("object=" + at->object +
                        " alias between pointer entry parameters unresolved in 6a");
                }
            }
            execute(*root, arguments, pointerArguments, {}, initial);
            if (result_.status == SolveStatus::Sat) break;
            ++rootIndex;
        }
        if (result_.status != SolveStatus::Sat) {
            result_.status = incomplete_ ? SolveStatus::Unknown : SolveStatus::Unsat;
            result_.reason = incomplete_ ? "Some feasible paths, dependencies or solver checks could not be fully modeled; no complete unreachability conclusion." :
                "No defined execution in the modeled caller scope reaches the target parameter with the requested value condition.";
        }
        resultDependencies_.insert(incompleteDependencies_.begin(), incompleteDependencies_.end());
        result_.unmodeledDependencies.assign(resultDependencies_.begin(), resultDependencies_.end());
        result_.unexploredPaths.assign(unexploredPaths_.begin(), unexploredPaths_.end());
        for (const auto& dependency : result_.unmodeledDependencies) {
            auto& category = discardedDependency(dependency) ? result_.discardedDependencies :
                result_.overapproximatedDependencies;
            category.push_back(dependency);
        }
        if (risk_ && !result_.discardedDependencies.empty()) {
            result_.status = SolveStatus::Unknown;
            result_.reason = "Discarded dependency or unexplored path prevents a risk verdict.";
        } else if (risk_ && result_.status == SolveStatus::Sat &&
                   std::any_of(result_.overapproximatedDependencies.begin(), result_.overapproximatedDependencies.end(),
                       [&](const auto& dependency) { return !declaredExternalInput(dependency, query_.externalInputSources); })) {
            result_.status = SolveStatus::Unknown;
            result_.reason = "SAT depends on an unmodeled value that is not a declared external input source.";
        } else if (risk_ && result_.status == SolveStatus::Sat && !result_.overapproximatedDependencies.empty()) {
            result_.reason = randMax_ ?
                "Risk is satisfiable for declared external input values; each rand call is within the configured range (范围已验证); PRNG sequence reachability is unverified." :
                "Risk is satisfiable for declared external input values; rand range unverified (范围未验证).";
        }
        if (risk_ && result_.status == SolveStatus::Unknown) {
            result_.inputs.clear();
            result_.operands.clear();
            result_.path.clear();
            result_.constraints.clear();
            result_.targetValue.clear();
            result_.resultIsPoison = false;
            result_.noDefinedResult = false;
        }
        result_.diagnostics.assign(notes_.begin(), notes_.end());
        result_.freeValues.assign(freeValues_.begin(), freeValues_.end());
        result_.randRangeBasis = randRangeBasis_;
        result_.memoryEvidence.assign(memoryEvidence_.begin(), memoryEvidence_.end());
        if (risk_ && result_.status == SolveStatus::Unsat) {
            if (result_.overapproximatedDependencies.empty())
                result_.reason = "No modeled path reaches this operation with the risk predicate true. This is a scoped result under the stated input domain.";
            else {
                result_.reason = "对以下未建模值的任意值成立: ";
                for (std::size_t i = 0; i < result_.overapproximatedDependencies.size(); ++i) {
                    if (i) result_.reason += "; ";
                    result_.reason += result_.overapproximatedDependencies[i];
                }
                result_.reason += "。风险在当前范围内不可触发。";
            }
        }
        return std::move(result_);
    }

private:
    bool budget() {
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started_).count();
        if (elapsed >= query_.timeoutMs) { unexplored("overall solve timeout"); return false; }
        if (steps_ >= query_.maxSteps) { unexplored("instruction step budget reached"); return false; }
        return result_.status != SolveStatus::Sat;
    }

    void unknown(const std::string& reason) {
        incomplete_ = true;
        notes_.insert(reason);
        incompleteDependencies_.insert(reason);
    }

    void unexplored(const std::string& reason) {
        unknown("unexplored path: " + reason);
        unexploredPaths_.insert(reason);
    }

    unsigned remainingTime() const {
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started_).count();
        return elapsed >= query_.timeoutMs ? 1U : query_.timeoutMs - static_cast<unsigned>(elapsed);
    }

    z3::check_result check(z3::solver& solver, const z3::expr& condition) {
        solver.set("timeout", remainingTime());
        solver.add(condition);
        const auto result = solver.check();
        if (result == z3::unknown) unknown("Z3: " + solver.reason_unknown());
        return result;
    }

    bool feasible(const z3::expr& condition) {
        z3::solver solver(z3_);
        return check(solver, condition) != z3::unsat;
    }

    void notePoisonUse(const State& state, const Value& operand, const llvm::Instruction& consumer,
                       const llvm::Value& source, const char* use) {
        z3::solver solver(z3_);
        if (check(solver, state.path.condition && !operand.valid) == z3::sat) {
            notes_.insert("Potential undefined behavior: poison used as " + std::string(use) + " in " +
                          symbolic::instructionText(consumer) + "; operand " + operandName(source) +
                          sourceSuffix(consumer));
        }
    }

    void enumerate(llvm::Function& function, unsigned remaining, Context& reverse) {
        if (!budget()) return;
        const auto found = callers_.find(&function);
        if (remaining > 0 && (found == callers_.end() || found->second.empty())) {
            notes_.insert("No direct callers retained for " + selection::functionName(function) +
                "; its arguments are unconstrained boundary inputs. Caller elimination or missing code cannot be inferred from this fact alone.");
        }
        if (remaining == 0 || found == callers_.end() || found->second.empty()) {
            if (targets_.size() >= query_.target.maxContexts) { unexplored("caller context budget reached"); return; }
            targets_.emplace_back(reverse.rbegin(), reverse.rend());
            if (remaining == 0 && found != callers_.end() && !found->second.empty()) result_.upstreamBoundary = true;
            return;
        }
        for (auto* caller : found->second) {
            reverse.push_back(caller);
            enumerate(*caller->getFunction(), remaining - 1, reverse);
            reverse.pop_back();
        }
    }

    unsigned distance(const Context& context) const {
        std::size_t common = 0;
        while (common < context.size() && common < expected_.size() && context[common] == expected_[common]) ++common;
        return static_cast<unsigned>(context.size() + expected_.size() - 2 * common);
    }

    Value unsupported(unsigned width, const std::string& reason) {
        notes_.insert(reason);
        const auto name = "unknown" + std::to_string(fresh_++);
        return {z3_.bv_const(name.c_str(), width), z3_.bool_val(true), z3_.bool_val(false), {reason}};
    }

    std::optional<PointerLocation> pointer(const llvm::Value& value, const State& state) const {
        if (auto found = state.pointers.find(&value); found != state.pointers.end()) return found->second;
        if (const auto* cast = llvm::dyn_cast<llvm::BitCastOperator>(&value))
            return pointer(*cast->getOperand(0), state);
        if (const auto* gep = llvm::dyn_cast<llvm::GEPOperator>(&value)) {
            auto base = pointer(*gep->getPointerOperand(), state);
            if (!base) return std::nullopt;
            llvm::APInt delta(module_.getDataLayout().getPointerSizeInBits(), 0);
            if (!gep->accumulateConstantOffset(module_.getDataLayout(), delta)) return std::nullopt;
            base->offset += delta.getSExtValue();
            return base;
        }
        return std::nullopt;
    }

    static std::string memoryKey(const PointerLocation& at, unsigned bytes) {
        return at.object + ":" + std::to_string(at.offset) + ":" + std::to_string(bytes);
    }

    void escape(Path& path, const PointerLocation& at, const std::string& reason) {
        path.escapedObjects.insert(at.object);
        path.memoryEvidence.insert("object=" + at.object + " offset=" + std::to_string(at.offset) +
            " address escaped at " + reason);
    }

    void escapeCandidates(Path& path, const llvm::Value& value, const State& state,
                          const std::string& reason, std::set<const llvm::Value*>& seen) {
        if (!seen.insert(&value).second) return;
        if (auto at = pointer(value, state)) { escape(path, *at, reason); return; }
        if (const auto* phi = llvm::dyn_cast<llvm::PHINode>(&value)) {
            for (const auto& incoming : phi->incoming_values())
                escapeCandidates(path, *incoming, state, reason, seen);
            return;
        }
        if (const auto* select = llvm::dyn_cast<llvm::SelectInst>(&value)) {
            escapeCandidates(path, *select->getTrueValue(), state, reason, seen);
            escapeCandidates(path, *select->getFalseValue(), state, reason, seen);
            return;
        }
        // A pointer loaded from memory may have passed through arbitrary stores.
        // Conservatively escape every location currently identifiable on this path.
        for (const auto& [_, at] : state.pointers) escape(path, at, reason);
    }

    Value read(const llvm::Value& value, State& state) {
        if (const auto found = state.values.find(&value); found != state.values.end()) return found->second;
        const auto width = value.getType()->getIntegerBitWidth();
        if (const auto* integer = llvm::dyn_cast<llvm::ConstantInt>(&value)) {
            llvm::SmallString<64> decimal;
            integer->getValue().toString(decimal, 10, false);
            return {z3_.bv_val(decimal.c_str(), width), z3_.bool_val(true), z3_.bool_val(true)};
        }
        if (llvm::isa<llvm::PoisonValue>(value)) {
            return {z3_.bv_val(0, width), z3_.bool_val(false), z3_.bool_val(true)};
        }
        return unsupported(width, "unresolved integer value: " + operandName(value));
    }

    void checkTarget(const Arguments& arguments, const Path& path) {
        memoryEvidence_.insert(path.memoryEvidence.begin(), path.memoryEvidence.end());
        ++result_.candidatesChecked;
        const auto& value = *arguments.at(parameter_);
        resultDependencies_.insert(path.unmodeledDependencies.begin(), path.unmodeledDependencies.end());
        resultDependencies_.insert(value.unmodeledDependencies.begin(), value.unmodeledDependencies.end());
        const auto condition = symbolic::predicate(value.bits, query_.comparison, query_.value);
        const auto known = path.known && value.known;
        z3::solver solver(z3_);
        if (check(solver, path.condition && value.valid && known && condition) == z3::sat) {
            const auto model = solver.get_model();
            result_.status = SolveStatus::Sat;
            result_.reason = "A modeled, defined caller path reaches the target condition. Inputs are boundary values, not a replayed execution.";
            result_.targetValue = numeral(model.eval(value.bits, true));
            for (const auto& input : rootInputs_) {
                result_.inputs.push_back(observe(model, input.name, input.bits));
            }
            result_.path = path.trace;
            result_.path.push_back("Target: " + selection::functionName(target_) + " arg" + std::to_string(parameter_) + " = " + result_.targetValue);
            result_.constraints = path.constraints;
            result_.constraints.push_back("Target condition: " + condition.simplify().to_string());
            result_.constraints.push_back("Defined-value condition: " + value.valid.simplify().to_string());
            return;
        }
        if (feasible(path.condition && value.valid && !known)) {
            unknown("target may be reachable with an unmodeled value or control effect");
        }
    }

    void checkRisk(State& state) {
        memoryEvidence_.insert(state.path.memoryEvidence.begin(), state.path.memoryEvidence.end());
        ++result_.candidatesChecked;
        const auto& instruction = *risk_->instruction;
        std::vector<Value> values;
        for (const auto& operand : instruction.operands()) values.push_back(read(*operand, state));
        if (instruction.getOpcode() == llvm::Instruction::UDiv || instruction.getOpcode() == llvm::Instruction::SDiv ||
            instruction.getOpcode() == llvm::Instruction::URem || instruction.getOpcode() == llvm::Instruction::SRem) {
            notePoisonUse(state, values.at(1), instruction, *instruction.getOperand(1), "division or remainder divisor");
        }
        resultDependencies_.insert(state.path.unmodeledDependencies.begin(), state.path.unmodeledDependencies.end());
        const auto condition = symbolic::riskCondition(instruction, risk_->reason, values);
        const bool divisorOnly = risk_->reason == ArithmeticRiskReason::DivideByZero;
        const bool amountOnly = risk_->reason == ArithmeticRiskReason::InvalidShiftAmount;
        auto valid = z3_.bool_val(true);
        for (std::size_t index = 0; index < values.size(); ++index) {
            if ((divisorOnly || amountOnly) && index == 0) continue;
            valid = valid && values[index].valid;
            resultDependencies_.insert(values[index].unmodeledDependencies.begin(), values[index].unmodeledDependencies.end());
        }
        z3::solver solver(z3_);
        // Never include binary(instruction).valid here: that would assume away
        // the very no-wrap violation/division failure being checked.
        if (check(solver, state.path.condition && valid && condition) == z3::sat) {
            const auto model = solver.get_model();
            result_.status = SolveStatus::Sat;
            result_.reason = "Risk predicate is satisfiable before the operation under the recorded boundary inputs. Source closure is not established.";
            for (const auto& input : rootInputs_) result_.inputs.push_back(observe(model, input.name, input.bits));
            for (const auto& call : state.path.randCalls) result_.randCalls.push_back(observe(model, call.name, call.bits));
            for (std::size_t index = 0; index < values.size(); ++index) {
                const auto name = "operand" + std::to_string(index);
                const auto& value = values[index];
                if (model.eval(value.valid, true).is_true()) result_.operands.push_back(observe(model, name, value.bits));
                else result_.operands.push_back({name, value.bits.get_sort().bv_size(), "<unmodeled-or-poison>", "<unmodeled-or-poison>"});
            }
            result_.noDefinedResult = divisorOnly || risk_->reason == ArithmeticRiskReason::SignedDivisionOverflow;
            if (amountOnly) {
                // LLVM shifts with out-of-range amounts produce poison. Z3's
                // total bit-vector shift result must not be reported as an IR value.
                result_.resultIsPoison = true;
            } else if (!result_.noDefinedResult) {
                const auto operation = llvm::isa<llvm::TruncInst>(instruction) ?
                    symbolic::cast(instruction, values[0], query_.noWrapSemantics == NoWrapSemantics::Wrap) :
                    symbolic::binary(instruction, values[0], values[1], query_.noWrapSemantics == NoWrapSemantics::Wrap);
                result_.targetValue = numeral(model.eval(operation.bits, true));
                result_.resultIsPoison = !model.eval(operation.valid, true).is_true();
            }
            result_.path = state.path.trace;
            result_.path.push_back("Risk operation: " + symbolic::instructionText(instruction) + sourceSuffix(instruction));
            result_.constraints = state.path.constraints;
            result_.constraints.push_back("Risk condition: " + condition.simplify().to_string());
            result_.constraints.push_back("Operand definedness before target: " + valid.simplify().to_string());
            notes_.insert("The target operation's own no-wrap/exact conditions were not assumed when checking its failure; earlier execution conditions remain enforced.");
            notes_.insert("Origin remains a function-entry boundary, including for roots with no retained callers. No source-confirmed classification is inferred.");
            return;
        }
    }

    bool enter(State& state, const llvm::BasicBlock& block, const llvm::BasicBlock* predecessor) {
        if (!budget()) return false;
        if (states_ >= query_.maxStates) { unexplored("path state budget reached"); return false; }
        if (!state.visited.insert(&block).second) { unexplored("loop back-edge reached at " + blockName(block)); return false; }
        ++states_;
        state.path.trace.push_back("Block: " + blockName(block));
        std::vector<std::pair<const llvm::Value*, Value>> incoming;
        for (const auto& instruction : block.phis()) {
            if (!instruction.getType()->isIntegerTy()) continue;
            const auto index = predecessor ? instruction.getBasicBlockIndex(predecessor) : -1;
            auto value = index >= 0 ? read(*instruction.getIncomingValue(static_cast<unsigned>(index)), state) :
                                     unsupported(instruction.getType()->getIntegerBitWidth(), "PHI predecessor unavailable");
            incoming.emplace_back(&instruction, value);
        }
        for (auto& [instruction, value] : incoming) state.values.insert_or_assign(instruction, value);
        state.next = &*block.getFirstNonPHIIt();
        return true;
    }

    void constrain(State& state, const Value& value, const z3::expr& condition, const std::string& label) {
        for (const auto& dependency : value.unmodeledDependencies)
            if (!declaredExternalInput(dependency, query_.externalInputSources))
                state.path.unmodeledDependencies.insert("unsupported control dependency: " + dependency);
        state.path.condition = state.path.condition && value.valid && condition;
        state.path.known = state.path.known && value.known;
        state.path.unmodeledDependencies.insert(value.unmodeledDependencies.begin(), value.unmodeledDependencies.end());
        state.path.constraints.push_back(label + ": " + condition.simplify().to_string());
        state.path.trace.push_back(label);
    }

    std::vector<Returned> execute(llvm::Function& function, const Arguments& arguments,
                                  const PointerArguments& pointerArguments, const Context& context, Path path) {
        if (!budget()) return {};
        for (unsigned index = 0; index < function.arg_size(); ++index) {
            if (function.getAttributes().getParamAttrs(index).hasAttribute(llvm::Attribute::NoUndef) && arguments.at(index)) {
                path.condition = path.condition && arguments.at(index)->valid;
            }
            if (function.getAttributes().getParamAttrs(index).hasAttribute(llvm::Attribute::Range)) {
                path.known = z3_.bool_val(false);
                const auto reason = "parameter range attribute not modeled in " + selection::functionName(function);
                path.unmodeledDependencies.insert(reason);
                notes_.insert(reason);
            }
        }
        if (!risk_ && &function == &target_ && context == expected_) { checkTarget(arguments, path); return {}; }
        const bool outsideScope = risk_ ? !risk_->allowedContexts.contains(context) : distance(context) > query_.target.upDepth;
        if (risk_ && function.isDeclaration() && function.getName() == "rand" &&
            function.getReturnType()->isIntegerTy()) {
            const auto label = "rand.call" + std::to_string(fresh_);
            auto value = unsupported(function.getReturnType()->getIntegerBitWidth(),
                "unmodeled return value from external call before target: rand");
            freeValues_.insert(label + " i" + std::to_string(function.getReturnType()->getIntegerBitWidth()));
            path.randCalls.push_back({label, value.bits});
            if (randMax_) {
                const auto width = function.getReturnType()->getIntegerBitWidth();
                path.condition = path.condition && z3::ule(value.bits, z3_.bv_val(*randMax_, width));
                path.constraints.push_back(label + " in [0," + std::to_string(*randMax_) + "]");
            }
            notes_.insert("rand calls are independent symbolic inputs; PRNG sequence correlations are not modeled.");
            return {{path, value}};
        }
        const bool unmodeledInputCall = function.getName() == "fscanf" || function.getName() == "fgets";
        if (function.isDeclaration() || outsideScope || unmodeledInputCall) {
            if (function.hasFnAttribute(llvm::Attribute::NoReturn)) return {};
            const auto reason = function.isDeclaration() ? "external call before target: " :
                risk_ ? "call dependency outside selected data slice: " : "call dependency beyond --up scope: ";
            notes_.insert(reason + selection::functionName(function));
            for (const auto& at : pointerArguments) if (at)
                escape(path, *at, reason + selection::functionName(function));
            path.possibleMemoryWrites.insert(std::string("unmodeled pointer/global write by ") + reason + selection::functionName(function));
            if (risk_ && context.size() <= expected_.size() && std::equal(context.begin(), context.end(), expected_.begin())) {
                unexplored("target call context cannot be expanded within the selected data slice");
            }
            std::optional<Value> result;
            if (function.getReturnType()->isIntegerTy()) result = unsupported(function.getReturnType()->getIntegerBitWidth(),
                std::string("unmodeled return value from ") + reason + selection::functionName(function));
            return {{path, result}};
        }
        State initial{path, {}, {}, {}, nullptr};
        for (auto& argument : function.args()) {
            if (argument.getType()->isIntegerTy() && arguments.at(argument.getArgNo())) {
                initial.values.emplace(&argument, *arguments.at(argument.getArgNo()));
            }
            if (argument.getType()->isPointerTy() && pointerArguments.at(argument.getArgNo()))
                initial.pointers.emplace(&argument, *pointerArguments.at(argument.getArgNo()));
        }
        std::deque<State> pending;
        if (enter(initial, function.getEntryBlock(), nullptr)) pending.push_back(std::move(initial));
        std::vector<Returned> returned;
        while (!pending.empty() && budget()) {
            auto state = std::move(pending.front());
            pending.pop_front();
            while (state.next && budget()) {
                ++steps_;
                const auto& instruction = *state.next;
                state.next = instruction.getNextNode();
                if (risk_ && &instruction == risk_->instruction && context == expected_) {
                    checkRisk(state);
                    break;
                }
                if (const auto* branch = llvm::dyn_cast<llvm::UncondBrInst>(&instruction)) {
                    if (enter(state, *branch->getSuccessor(), instruction.getParent())) pending.push_back(std::move(state));
                    break;
                }
                if (const auto* branch = llvm::dyn_cast<llvm::CondBrInst>(&instruction)) {
                    const auto value = read(*branch->getCondition(), state);
                    notePoisonUse(state, value, instruction, *branch->getCondition(), "branch condition");
                    for (unsigned index = 0; index != 2; ++index) {
                        auto successor = state;
                        const auto condition = index == 0 ? value.bits == 1 : value.bits == 0;
                        constrain(successor, value, condition, "Branch " + std::string(index == 0 ? "true" : "false") + sourceSuffix(instruction));
                        if (feasible(successor.path.condition) && enter(successor, *branch->getSuccessor(index), instruction.getParent())) pending.push_back(std::move(successor));
                    }
                    break;
                }
                if (const auto* instructionSwitch = llvm::dyn_cast<llvm::SwitchInst>(&instruction)) {
                    const auto value = read(*instructionSwitch->getCondition(), state);
                    notePoisonUse(state, value, instruction, *instructionSwitch->getCondition(), "switch condition");
                    auto defaultCondition = z3_.bool_val(true);
                    for (const auto& caseValue : instructionSwitch->cases()) {
                        auto successor = state;
                        const auto condition = value.bits == read(*caseValue.getCaseValue(), state).bits;
                        defaultCondition = defaultCondition && !condition;
                        constrain(successor, value, condition, "Switch case " + operandName(*caseValue.getCaseValue()) + sourceSuffix(instruction));
                        if (feasible(successor.path.condition) && enter(successor, *caseValue.getCaseSuccessor(), instruction.getParent())) pending.push_back(std::move(successor));
                    }
                    constrain(state, value, defaultCondition, "Switch default" + sourceSuffix(instruction));
                    if (feasible(state.path.condition) && enter(state, *instructionSwitch->getDefaultDest(), instruction.getParent())) pending.push_back(std::move(state));
                    break;
                }
                if (const auto* ret = llvm::dyn_cast<llvm::ReturnInst>(&instruction)) {
                    if (function.hasFnAttribute(llvm::Attribute::NoReturn)) break;
                    std::optional<Value> value;
                    if (ret->getReturnValue() && ret->getReturnValue()->getType()->isIntegerTy()) value = read(*ret->getReturnValue(), state);
                    if (value && function.getAttributes().hasRetAttr(llvm::Attribute::NoUndef)) state.path.condition = state.path.condition && value->valid;
                    returned.push_back({state.path, value});
                    break;
                }
                if (llvm::isa<llvm::UnreachableInst>(instruction)) break;
                if (const auto* call = llvm::dyn_cast<llvm::CallInst>(&instruction)) {
                    if (handleIntrinsic(*call, state)) {
                        if (!feasible(state.path.condition)) break;
                        continue;
                    }
                    auto* callee = directCallee(*call);
                    if (!callee) {
                        const auto reason = "indirect call before target" + sourceSuffix(instruction);
                        state.path.possibleMemoryWrites.insert("unmodeled pointer/global write by " + reason);
                        notes_.insert(reason);
                        for (const auto& operand : call->args()) if (operand->getType()->isPointerTy()) {
                            std::set<const llvm::Value*> seen;
                            escapeCandidates(state.path, *operand, state, reason, seen);
                        }
                        if (call->getType()->isIntegerTy()) {
                            state.values.insert_or_assign(call, unsupported(call->getType()->getIntegerBitWidth(),
                                "indirect call result: " + symbolic::instructionText(instruction)));
                        }
                        if (call->doesNotReturn()) break;
                        continue;
                    }
                    if (call->hasOperandBundles()) {
                        state.path.known = z3_.bool_val(false);
                        state.path.unmodeledDependencies.insert("call operand bundles before target are not modeled");
                        notes_.insert("call operand bundles before target are not modeled");
                    }
                    Arguments actual;
                    PointerArguments actualPointers;
                    for (const auto& argument : call->args()) {
                        actual.push_back(argument->getType()->isIntegerTy() ? std::optional<Value>(read(*argument, state)) : std::nullopt);
                        auto at = argument->getType()->isPointerTy() ? pointer(*argument, state) : std::nullopt;
                        if (argument->getType()->isPointerTy() && !at) {
                            std::set<const llvm::Value*> seen;
                            escapeCandidates(state.path, *argument, state,
                                "unresolved pointer call argument" + sourceSuffix(instruction), seen);
                        }
                        actualPointers.push_back(std::move(at));
                    }
                    for (unsigned index = 0; index < actual.size(); ++index) {
                        if (actual[index] && call->paramHasAttr(index, llvm::Attribute::NoUndef)) state.path.condition = state.path.condition && actual[index]->valid;
                        if (call->paramHasAttr(index, llvm::Attribute::Range)) {
                            state.path.known = z3_.bool_val(false);
                            state.path.unmodeledDependencies.insert("call-site parameter range attribute not modeled");
                            notes_.insert("call-site parameter range attribute not modeled");
                        }
                    }
                    auto childContext = context;
                    childContext.push_back(call);
                    state.path.trace.push_back("Call: " + selection::functionName(*callee) + sourceSuffix(instruction));
                    const auto cases = execute(*callee, actual, actualPointers, childContext, state.path);
                    for (const auto& outcome : cases) {
                        auto continuation = state;
                        continuation.path = outcome.path;
                        if (call->getType()->isIntegerTy()) {
                            auto value = outcome.value ? *outcome.value : unsupported(call->getType()->getIntegerBitWidth(), "integer call result not modeled");
                            if (call->hasRetAttr(llvm::Attribute::Range)) {
                                value.known = z3_.bool_val(false);
                                value.unmodeledDependencies.insert("call result range attribute not modeled");
                                notes_.insert("call result range attribute not modeled");
                            }
                            if (call->hasRetAttr(llvm::Attribute::NoUndef)) continuation.path.condition = continuation.path.condition && value.valid;
                            continuation.values.insert_or_assign(call, value);
                        }
                        pending.push_back(std::move(continuation));
                    }
                    break;
                }
                if (instruction.isTerminator()) { unexplored("unsupported control transfer: " + symbolic::instructionText(instruction)); break; }
                if (const auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(&instruction)) {
                    state.pointers.emplace(alloca, PointerLocation{selection::functionName(function) +
                        ":" + operandName(*alloca) + ":" + std::to_string(fresh_++), 0});
                    continue;
                }
                if (const auto* store = llvm::dyn_cast<llvm::StoreInst>(&instruction)) {
                    if (store->getValueOperand()->getType()->isPointerTy()) {
                        std::set<const llvm::Value*> seen;
                        escapeCandidates(state.path, *store->getValueOperand(), state,
                            "pointer stored to memory" + sourceSuffix(instruction), seen);
                    } else if (store->getValueOperand()->getType()->isIntegerTy()) {
                        auto at = pointer(*store->getPointerOperand(), state);
                        if (at) {
                            const auto bytes = (store->getValueOperand()->getType()->getIntegerBitWidth() + 7) / 8;
                            const auto location = memoryKey(*at, bytes);
                            if (auto prior = state.path.singleLocationByObject.find(at->object);
                                prior != state.path.singleLocationByObject.end() && prior->second != location)
                                state.path.multiLocationObjects.insert(at->object);
                            else state.path.singleLocationByObject[at->object] = location;
                            state.path.memory.insert_or_assign(memoryKey(*at, bytes),
                                MemorySlot{read(*store->getValueOperand(), state),
                                    selection::functionName(function) + sourceSuffix(instruction)});
                        } else {
                            state.path.possibleMemoryWrites.insert("unresolved pointer store" + sourceSuffix(instruction));
                            std::set<const llvm::Value*> seen;
                            escapeCandidates(state.path, *store->getPointerOperand(), state,
                                "unresolved pointer store" + sourceSuffix(instruction), seen);
                        }
                    }
                    continue;
                }
                if (instruction.getType()->isIntegerTy()) {
                    state.values.insert_or_assign(&instruction, evaluate(instruction, state));
                } else if (instruction.mayReadOrWriteMemory()) {
                    const auto reason = "memory side effect before target not modeled" + sourceSuffix(instruction);
                    state.path.possibleMemoryWrites.insert(reason);
                    notes_.insert(reason);
                }
            }
        }
        return returned;
    }

    bool handleIntrinsic(const llvm::CallInst& call, State& state) {
        const auto* callee = directCallee(call);
        if (!callee || !callee->isIntrinsic()) return false;
        const auto name = callee->getName();
        if (name.starts_with("llvm.dbg.") || name.starts_with("llvm.lifetime.")) return true;
        if (name.starts_with("llvm.memset.")) {
            if (auto at = pointer(*call.getArgOperand(0), state)) {
                for (auto it = state.path.memory.begin(); it != state.path.memory.end();) {
                    if (it->first.starts_with(at->object + ":")) it = state.path.memory.erase(it);
                    else ++it;
                }
                state.path.havocObjects.insert(at->object);
                notes_.insert("llvm.memset is conservatively treated as an unknown write before a later strong store");
            }
            return true;
        }
        if (name == "llvm.assume") {
            const auto value = read(*call.getArgOperand(0), state);
            constrain(state, value, value.bits == 1, "llvm.assume" + sourceSuffix(call));
            if (call.hasOperandBundles()) {
                state.path.known = z3_.bool_val(false);
                state.path.unmodeledDependencies.insert("llvm.assume operand bundles are not modeled");
                notes_.insert("llvm.assume operand bundles are not modeled");
            }
            return true;
        }
        if (name.starts_with("llvm.abs.") && call.getType()->isIntegerTy()) {
            const auto input = read(*call.getArgOperand(0), state);
            const auto poisonFlag = read(*call.getArgOperand(1), state);
            const auto width = input.bits.get_sort().bv_size();
            const auto minimum = z3::shl(z3_.bv_val(1, width), z3_.bv_val(width - 1, width));
            const auto negative = input.bits < z3_.bv_val(0, width);
            state.values.insert_or_assign(&call, Value{
                z3::ite(negative, -input.bits, input.bits),
                input.valid && poisonFlag.valid && !(poisonFlag.bits == 1 && input.bits == minimum),
                input.known && poisonFlag.known, symbolic::mergeDependencies(input, poisonFlag)});
            return true;
        }
        if ((name.starts_with("llvm.smin.") || name.starts_with("llvm.smax.") ||
             name.starts_with("llvm.umin.") || name.starts_with("llvm.umax.")) && call.getType()->isIntegerTy()) {
            const auto a = read(*call.getArgOperand(0), state);
            const auto b = read(*call.getArgOperand(1), state);
            const bool signedOp = name.starts_with("llvm.s");
            const auto less = signedOp ? a.bits < b.bits : z3::ult(a.bits, b.bits);
            const auto chooseFirst = name.contains("min.") ? less : !less;
            state.values.insert_or_assign(&call, Value{z3::ite(chooseFirst, a.bits, b.bits), a.valid && b.valid,
                a.known && b.known, symbolic::mergeDependencies(a, b)});
            return true;
        }
        return false;
    }

    Value evaluate(const llvm::Instruction& instruction, State& state) {
        if (const auto* load = llvm::dyn_cast<llvm::LoadInst>(&instruction)) {
            const auto width = instruction.getType()->getIntegerBitWidth();
            const auto bytes = (width + 7) / 8;
            const auto at = pointer(*load->getPointerOperand(), state);
            if (!at) return unsupported(width, "unresolved memory pointer at load" + sourceSuffix(instruction));
            const auto key = memoryKey(*at, bytes);
            if (state.path.multiLocationObjects.contains(at->object)) {
                state.path.memoryEvidence.insert("object=" + at->object + " offset=" +
                    std::to_string(at->offset) + " size=" + std::to_string(bytes) +
                    " last-write=multiple locations update=havoc");
                return unsupported(width, "6a multiple locations in one object: " + key);
            }
            if (state.path.escapedObjects.contains(at->object)) {
                state.path.memoryEvidence.insert("object=" + at->object + " offset=" +
                    std::to_string(at->offset) + " size=" + std::to_string(bytes) +
                    " last-write=unknown update=havoc");
                return unsupported(width, "地址已逃逸（待 6b）: " + key);
            }
            if (auto found = state.path.memory.find(key); found != state.path.memory.end()) {
                state.path.memoryEvidence.insert("object=" + at->object + " offset=" +
                    std::to_string(at->offset) + " size=" + std::to_string(bytes) +
                    " last-write=" + found->second.lastWrite + " update=strong");
                return found->second.value;
            }
            if (state.path.havocObjects.contains(at->object)) {
                state.path.memoryEvidence.insert("object=" + at->object + " offset=" +
                    std::to_string(at->offset) + " size=" + std::to_string(bytes) +
                    " last-write=unmodeled llvm.memset update=havoc");
                return unsupported(width, "unmodeled llvm.memset write: " + key);
            }
            state.path.memoryEvidence.insert("object=" + at->object + " offset=" +
                std::to_string(at->offset) + " size=" + std::to_string(bytes) +
                " last-write=entry update=entry-symbol");
            const auto symbol = "entry_memory" + std::to_string(fresh_++);
            bool selectedEntry = false;
            for (auto index : query_.selectedPointerArguments)
                if (key.starts_with("root0.arg" + std::to_string(index) + ":") &&
                    query_.selectedPointerFunction == target_.getName()) selectedEntry = true;
            Value entry = selectedEntry ?
                Value{z3_.bv_const(symbol.c_str(), width), z3_.bool_val(true), z3_.bool_val(true)} :
                unsupported(width, "unmodeled entry memory value: " + key);
            state.path.memory.insert_or_assign(key, MemorySlot{entry, "entry"});
            freeValues_.insert(symbol + " i" + std::to_string(width));
            rootInputs_.push_back({key, entry.bits});
            return entry;
        }
        if (instruction.isBinaryOp()) {
            const auto left = read(*instruction.getOperand(0), state);
            const auto right = read(*instruction.getOperand(1), state);
            const auto opcode = instruction.getOpcode();
            if (opcode == llvm::Instruction::UDiv || opcode == llvm::Instruction::SDiv ||
                opcode == llvm::Instruction::URem || opcode == llvm::Instruction::SRem) {
                // Division by zero and signed division overflow are immediate
                // UB even when the result is unused. No-wrap poison is not.
                notePoisonUse(state, right, instruction, *instruction.getOperand(1), "division or remainder divisor");
                auto defined = right.valid && right.bits != 0;
                if (opcode == llvm::Instruction::SDiv || opcode == llvm::Instruction::SRem) {
                    const auto width = left.bits.get_sort().bv_size();
                    const auto min = z3::shl(z3_.bv_val(1, width), z3_.bv_val(width - 1, width));
                    defined = defined && z3::implies(right.bits == -1, left.valid && left.bits != min);
                }
                state.path.condition = state.path.condition && defined;
                state.path.known = state.path.known && right.known && left.known;
                state.path.unmodeledDependencies.insert(left.unmodeledDependencies.begin(), left.unmodeledDependencies.end());
                state.path.unmodeledDependencies.insert(right.unmodeledDependencies.begin(), right.unmodeledDependencies.end());
            }
            return symbolic::binary(instruction, left, right,
                query_.noWrapSemantics == NoWrapSemantics::Wrap);
        }
        if (llvm::isa<llvm::ICmpInst>(instruction) && instruction.getOperand(0)->getType()->isIntegerTy()) {
            return symbolic::compare(instruction, read(*instruction.getOperand(0), state), read(*instruction.getOperand(1), state));
        }
        if (llvm::isa<llvm::TruncInst>(instruction) || llvm::isa<llvm::ZExtInst>(instruction) || llvm::isa<llvm::SExtInst>(instruction)) {
            return symbolic::cast(instruction, read(*instruction.getOperand(0), state),
                query_.noWrapSemantics == NoWrapSemantics::Wrap);
        }
        if (const auto* choice = llvm::dyn_cast<llvm::SelectInst>(&instruction)) {
            return symbolic::select(read(*choice->getCondition(), state), read(*choice->getTrueValue(), state), read(*choice->getFalseValue(), state));
        }
        auto value = unsupported(instruction.getType()->getIntegerBitWidth(),
            "unsupported value dependency: " + symbolic::instructionText(instruction) + sourceSuffix(instruction));
        if (llvm::isa<llvm::LoadInst>(instruction)) {
            value.unmodeledDependencies.insert(state.path.possibleMemoryWrites.begin(),
                                               state.path.possibleMemoryWrites.end());
        }
        return value;
    }

    llvm::Module& module_;
    const ReachabilityQuery& query_;
    llvm::Function& target_;
    const execution::InstructionRiskTarget* risk_;
    unsigned parameter_ = 0;
    z3::context z3_;
    std::chrono::steady_clock::time_point started_;
    ReachabilityResult result_;
    std::map<const llvm::Function*, std::vector<llvm::CallBase*>> callers_;
    std::vector<Context> targets_;
    Context expected_;
    std::vector<Observed> rootInputs_;
    std::set<std::string> freeValues_;
    std::optional<unsigned> randMax_;
    std::string randRangeBasis_;
    std::set<std::string> notes_;
    std::set<std::string> resultDependencies_;
    std::set<std::string> incompleteDependencies_;
    std::set<std::string> unexploredPaths_;
    std::set<std::string> memoryEvidence_;
    std::size_t steps_ = 0, states_ = 0, fresh_ = 0;
    bool incomplete_ = false;
};

} // namespace

ReachabilityResult solveParameterReachability(const std::filesystem::path& file, const ReachabilityQuery& query) {
    return solveParameterReachability(std::vector<std::filesystem::path>{file}, query);
}
ReachabilityResult solveParameterReachability(const std::vector<std::filesystem::path>& files, const ReachabilityQuery& query) {
    llvm::LLVMContext context;
    auto module = selection::loadModule(files, context);
    return Executor(*module, query).run();
}

namespace execution {

ReachabilityResult solveInstructionRisk(llvm::Module& module, const ReachabilityQuery& limits,
                                       const InstructionRiskTarget& target) {
    return Executor(module, limits, &target).run();
}

} // namespace execution

} // namespace valuetrace
