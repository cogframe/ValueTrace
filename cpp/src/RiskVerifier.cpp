#include "valuetrace/RiskVerifier.h"
#include "InstructionRisk.h"
#include "LlvmSupport.h"

#include <llvm/ADT/SmallString.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <algorithm>
#include <chrono>
#include <limits>
#include <set>
#include <stdexcept>

namespace valuetrace {
namespace {

using IdentityPath = std::vector<CallSiteIdentity>;
using execution::CallContext;

bool prefix(const IdentityPath& path, const IdentityPath& other, std::size_t count) {
    return path.size() >= count && other.size() >= count && std::equal(path.begin(), path.begin() + count, other.begin());
}

llvm::Instruction& instructionAt(llvm::Function& function, unsigned ordinal) {
    unsigned index = 0;
    for (auto& block : function) for (auto& instruction : block) {
        if (index++ == ordinal) return instruction;
    }
    throw std::runtime_error("instruction identity no longer matches the input module");
}

CallContext resolvePath(llvm::Module& module, const std::string& root, const IdentityPath& path) {
    auto* function = module.getFunction(root);
    if (!function) throw std::runtime_error("root function identity missing");
    CallContext result;
    for (const auto& site : path) {
        if (function->getName() != site.caller) throw std::runtime_error("caller identity mismatch");
        auto* call = llvm::dyn_cast<llvm::CallBase>(&instructionAt(*function, site.instructionOrdinal));
        if (!call) throw std::runtime_error("call site identity does not identify a call");
        function = llvm::dyn_cast<llvm::Function>(call->getCalledOperand()->stripPointerCasts());
        if (!function) throw std::runtime_error("call site target is not a direct function");
        result.push_back(call);
    }
    return result;
}

execution::InstructionRiskTarget targetFor(llvm::Module& module, const ImportedProgram& program,
                                           const SliceResult& slice, NodeId id, ArithmeticRiskReason reason,
                                           bool selectReturn) {
    const auto& node = program.graph.node(id);
    if (!node.instructionOrdinal) throw std::runtime_error("candidate has no stable instruction identity");
    auto* function = module.getFunction(node.function);
    if (!function) throw std::runtime_error("candidate function is unavailable");
    auto& instruction = instructionAt(*function, *node.instructionOrdinal);
    if (node.operation.opcode != instruction.getOpcodeName() || !instruction.getType()->isIntegerTy() ||
        instruction.getType()->getIntegerBitWidth() != node.bitWidth) throw std::runtime_error("candidate instruction identity mismatch");

    // The importer retains one extra caller for displaying depth boundaries.
    // Trim it: verification must not silently use that caller's constraints.
    auto cut = std::numeric_limits<std::size_t>::max();
    for (auto seed : program.query.parameters) {
        const auto& selected = program.graph.node(seed);
        if (selected.rootFunction != node.rootFunction) continue;
        if (selectReturn && node.callPath.size() < selected.callPath.size() &&
            prefix(selected.callPath, node.callPath, node.callPath.size())) {
            cut = std::min(cut, std::size_t{0});
            continue;
        }
        const auto depth = selected.callPath.size();
        const auto candidate = depth > program.query.upDepth ? depth - program.query.upDepth : 0;
        if (prefix(node.callPath, selected.callPath, candidate)) cut = std::min(cut, candidate);
    }
    if (cut == std::numeric_limits<std::size_t>::max()) {
        throw std::runtime_error("candidate requires a caller continuation outside the requested upstream boundary; cannot safely reconstruct its entry state");
    }
    const auto absolute = resolvePath(module, node.rootFunction, node.callPath);
    // A no-argument source call carries no value from its caller. Start at that
    // source function, rather than exploring unrelated wrapper contexts.
    std::string closureReason;
    for (std::size_t index = cut; index < absolute.size(); ++index) {
        auto* callee = llvm::dyn_cast<llvm::Function>(absolute[index]->getCalledOperand()->stripPointerCasts());
        if (callee && callee->arg_empty() && absolute[index]->arg_empty()) {
            cut = index + 1;
            closureReason = "source function has no parameters and its caller passes no data";
        }
        else if (absolute[index]->arg_size() > 0 &&
                 std::all_of(absolute[index]->arg_begin(), absolute[index]->arg_end(),
                     [](const llvm::Use& argument) { return llvm::isa<llvm::ConstantInt>(argument.get()); })) {
            cut = index;
            closureReason = "all call arguments are integer constants";
        }
    }
    execution::InstructionRiskTarget target{&instruction, reason, {absolute.begin() + cut, absolute.end()}, {}};
    llvm::Function* root = cut == 0 ? module.getFunction(node.rootFunction) :
        llvm::dyn_cast<llvm::Function>(absolute[cut - 1]->getCalledOperand()->stripPointerCasts());
    if (root && (root->arg_empty() || !closureReason.empty())) {
        target.closedUpDepth = static_cast<unsigned>(absolute.size() - cut);
        target.closureReason = closureReason.empty() ?
            "source function has no parameters and its caller passes no data" : closureReason;
    }
    const auto addContext = [&](NodeId included) {
        const auto& current = program.graph.node(included);
        if (current.rootFunction != node.rootFunction || !prefix(current.callPath, node.callPath, cut)) return;
        const auto path = resolvePath(module, current.rootFunction, current.callPath);
        target.allowedContexts.insert({path.begin() + cut, path.end()});
    };
    for (const auto& reached : slice.upstream.nodes) addContext(reached.node);
    for (const auto& reached : slice.downstream.nodes) addContext(reached.node);
    return target;
}

RiskVerification evidenceFrom(ReachabilityResult result) {
    RiskVerification evidence;
    evidence.solverResult = result.status == SolveStatus::Sat ? "SAT" : result.status == SolveStatus::Unsat ? "UNSAT" : "UNKNOWN";
    evidence.detail = std::move(result.reason);
    evidence.inputs = std::move(result.inputs);
    evidence.operands = std::move(result.operands);
    evidence.randCalls = std::move(result.randCalls);
    evidence.freeValues = std::move(result.freeValues);
    evidence.memoryEvidence = std::move(result.memoryEvidence);
    evidence.randRangeBasis = std::move(result.randRangeBasis);
    evidence.path = std::move(result.path);
    evidence.constraints = std::move(result.constraints);
    evidence.notes = std::move(result.diagnostics);
    evidence.unmodeledDependencies = std::move(result.unmodeledDependencies);
    evidence.overapproximatedDependencies = std::move(result.overapproximatedDependencies);
    evidence.discardedDependencies = std::move(result.discardedDependencies);
    evidence.unexploredPaths = std::move(result.unexploredPaths);
    evidence.resultBitPattern = std::move(result.targetValue);
    evidence.resultIsPoison = result.resultIsPoison;
    evidence.noDefinedResult = result.noDefinedResult;
    return evidence;
}

std::vector<std::string> boundaryCallerHints(const llvm::Module& module, const llvm::Function& boundary) {
    std::vector<std::string> hints;
    for (const auto& function : module) for (const auto& block : function) for (const auto& instruction : block) {
        const auto* call = llvm::dyn_cast<llvm::CallBase>(&instruction);
        if (!call || call->getCalledOperand()->stripPointerCasts() != &boundary) continue;
        std::string hint = "Boundary caller (N+1): " + selection::functionName(function);
        if (const auto& location = call->getDebugLoc())
            hint += " at " + location->getFilename().str() + ":" + std::to_string(location.getLine());
        for (unsigned i = 0; i < call->arg_size(); ++i) {
            const auto* value = call->getArgOperand(i);
            hint += " actual" + std::to_string(i) + "=";
            if (const auto* constant = llvm::dyn_cast<llvm::ConstantInt>(value)) {
                llvm::SmallString<40> decimal;
                constant->getValue().toString(decimal, 10, true);
                hint += "constant " + decimal.str().str();
            } else if (const auto* producer = llvm::dyn_cast<llvm::CallBase>(value)) {
                const auto* callee = llvm::dyn_cast<llvm::Function>(producer->getCalledOperand()->stripPointerCasts());
                hint += callee && callee->isDeclaration() ? "from external input " + callee->getName().str() : "local value";
            } else if (llvm::isa<llvm::Argument>(value) || llvm::isa<llvm::BinaryOperator>(value) ||
                       llvm::isa<llvm::CastInst>(value)) hint += "local value";
            else hint += "unable to determine";
        }
        hints.push_back(std::move(hint));
    }
    return hints;
}

} // namespace

RiskVerificationReport verifyArithmeticRisks(const std::filesystem::path& file, const ParameterQuery& scope,
                                            const VerificationOptions& options) {
    return verifyArithmeticRisks(std::vector<std::filesystem::path>{file}, scope, options);
}
RiskVerificationReport verifyArithmeticRisks(const std::vector<std::filesystem::path>& files, const ParameterQuery& scope,
                                            const VerificationOptions& options) {
    if (options.timeoutMs == 0 || options.maxStates == 0 || options.maxSteps == 0) throw std::invalid_argument("verification limits must be positive");
    const auto started = std::chrono::steady_clock::now();
    RiskVerificationReport report;
    report.program = readParameterGraph(files, scope);
    report.slice = buildSlice(report.program.graph, report.program.query);
    for (auto& note : report.program.diagnostics) note = "Graph import: " + note;
    llvm::LLVMContext context;
    auto module = selection::loadModule(files, context);
    auto& selectedFunction = selection::resolveFunction(*module, scope.function);
    std::vector<std::string> pointerSelectionNotes;
    const auto selectedIndices = scope.selectReturn ? std::vector<unsigned>{} :
        selection::resolveParameters(selection::describeFunction(selectedFunction),
            scope.parameters, scope.parameterNames, pointerSelectionNotes);
    std::vector<unsigned> selectedPointerArguments;
    for (auto index : selectedIndices)
        if (selectedFunction.getArg(index)->getType()->isPointerTy()) selectedPointerArguments.push_back(index);
    std::set<NodeId> included;
    std::set<std::string> verificationBoundaries;
    for (const auto& reached : report.slice.upstream.nodes) included.insert(reached.node);
    for (const auto& reached : report.slice.downstream.nodes) included.insert(reached.node);
    for (auto id : included) {
        const auto& node = report.program.graph.node(id);
        for (std::size_t index = 0; index < node.arithmeticRisks.size(); ++index) {
            ++report.candidates;
            RiskVerification evidence;
            evidence.solverResult = "UNKNOWN";
            try {
                const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started).count();
                if (elapsed >= options.timeoutMs) throw std::runtime_error("verification batch timeout; this candidate could not be completed");
                const auto target = targetFor(*module, report.program, report.slice, id,
                    node.arithmeticRisks[index].reason, scope.selectReturn);
                ReachabilityQuery limits;
                limits.target.function = node.function;
                limits.target.upDepth = scope.upDepth;
                limits.timeoutMs = options.timeoutMs - static_cast<unsigned>(elapsed);
                limits.maxStates = options.maxStates;
                limits.maxSteps = options.maxSteps;
                limits.externalInputSources = options.externalInputSources;
                limits.randMaxOverride = options.randMaxOverride;
                limits.randUnbounded = options.randUnbounded;
                limits.noWrapSemantics = options.noWrapSemantics;
                limits.selectedPointerFunction = selectedFunction.getName().str();
                limits.selectedPointerArguments = selectedPointerArguments;
                ++report.solverChecks;
                evidence = evidenceFrom(execution::solveInstructionRisk(*module, limits, target));
                evidence.noWrapSemantics = options.noWrapSemantics == NoWrapSemantics::Wrap ? "wrap" : "ub-free";
                if (options.noWrapSemantics == NoWrapSemantics::UbFree && evidence.solverResult == "UNSAT" &&
                    evidence.discardedDependencies.empty()) {
                    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - started).count();
                    if (now >= options.timeoutMs) {
                        evidence.solverResult = "UNKNOWN";
                        evidence.detail = "Wrap confirmation timed out after ub-free UNSAT.";
                        evidence.discardedDependencies.push_back("wrap confirmation timeout");
                    } else {
                        limits.timeoutMs = options.timeoutMs - static_cast<unsigned>(now);
                        limits.noWrapSemantics = NoWrapSemantics::Wrap;
                        ++report.solverChecks;
                        auto wrapEvidence = evidenceFrom(execution::solveInstructionRisk(*module, limits, target));
                        wrapEvidence.noWrapSemantics = "wrap";
                        if (wrapEvidence.solverResult == "UNSAT" && wrapEvidence.discardedDependencies.empty()) {
                            wrapEvidence.notes.insert(wrapEvidence.notes.end(), evidence.notes.begin(), evidence.notes.end());
                            wrapEvidence.notes.push_back("Wrap confirmation: UNSAT; ub-free was also UNSAT.");
                            evidence = std::move(wrapEvidence);
                        } else if (wrapEvidence.solverResult == "SAT" && wrapEvidence.discardedDependencies.empty()) {
                            evidence.solverResult = "UNKNOWN";
                            evidence.detail = "仅在无 UB 假设下不可触发；wrap 模式下存在触发输入。";
                            evidence.notes.push_back("Wrap confirmation: SAT; scoped-unreachable is not justified.");
                        } else {
                            evidence = std::move(wrapEvidence);
                            evidence.solverResult = "UNKNOWN";
                            evidence.detail = "Wrap confirmation incomplete after ub-free UNSAT.";
                        }
                    }
                }
                if (!target.closedUpDepth) {
                    const auto* boundary = target.context.empty() ? module->getFunction(node.function) :
                        target.context.front()->getFunction();
                    if (boundary) {
                        const auto hints = boundaryCallerHints(*module, *boundary);
                        evidence.notes.insert(evidence.notes.end(), hints.begin(), hints.end());
                    }
                }
                evidence.notes.push_back("Verification is limited to the union of selected parameter dependency contexts. Control-only helper calls outside that slice remain unmodeled.");
            } catch (const std::exception& error) {
                evidence = RiskVerification{};
                evidence.noWrapSemantics = options.noWrapSemantics == NoWrapSemantics::Wrap ? "wrap" : "ub-free";
                evidence.solverResult = "UNKNOWN";
                evidence.detail = error.what();
                evidence.unmodeledDependencies.push_back(error.what());
                evidence.discardedDependencies.push_back(error.what());
                if (evidence.detail.find("timeout") != std::string::npos ||
                    evidence.detail.find("budget") != std::string::npos)
                    evidence.unexploredPaths.push_back(error.what());
                evidence.notes.push_back("No unreachable or source-confirmed result was inferred from incomplete verification.");
            }
            const auto status = !evidence.discardedDependencies.empty() ? ReportStatus::Unknown :
                                evidence.solverResult == "SAT" ? ReportStatus::BoundaryTriggerable :
                                evidence.solverResult == "UNSAT" ? ReportStatus::ScopedUnreachable : ReportStatus::Unknown;
            if (status == ReportStatus::Unknown) {
                verificationBoundaries.insert("Verification boundary in " + node.function + " (" + node.context + "): " + evidence.detail);
                for (const auto& note : evidence.notes) verificationBoundaries.insert("Verification model note: " + note);
            }
            report.program.graph.setRiskVerification(id, index, status, std::move(evidence));
        }
    }
    report.program.diagnostics.insert(report.program.diagnostics.end(), verificationBoundaries.begin(), verificationBoundaries.end());
    report.program.diagnostics.push_back("Operation-specific verification completed before display filtering. SAT remains boundary-triggerable; source closure is not inferred from a root with no callers.");
    return report;
}

} // namespace valuetrace
