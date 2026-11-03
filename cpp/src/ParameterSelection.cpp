#include "LlvmSupport.h"

#include <llvm/BinaryFormat/Dwarf.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugProgramInstruction.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <cstdlib>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace valuetrace::selection {
namespace {

std::string typeText(const llvm::Type& type) {
    std::string text;
    llvm::raw_string_ostream stream(text);
    type.print(stream);
    return text;
}

std::string sourceType(const llvm::DIType* type, unsigned depth = 0) {
    if (!type || depth > 8) return "<unavailable>";
    if (!type->getName().empty()) return type->getName().str();
    if (const auto* derived = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
        const auto base = sourceType(derived->getBaseType(), depth + 1);
        switch (derived->getTag()) {
        case llvm::dwarf::DW_TAG_pointer_type: return base + "*";
        case llvm::dwarf::DW_TAG_reference_type: return base + "&";
        case llvm::dwarf::DW_TAG_rvalue_reference_type: return base + "&&";
        case llvm::dwarf::DW_TAG_const_type: return "const " + base;
        case llvm::dwarf::DW_TAG_volatile_type: return "volatile " + base;
        default: return base;
        }
    }
    return "<unnamed source type>";
}

SourceLocation sourceLocation(const llvm::DISubprogram& subprogram) {
    auto path = std::filesystem::path(subprogram.getFilename().str());
    if (path.is_relative()) path = std::filesystem::path(subprogram.getDirectory().str()) / path;
    return {path.generic_string(), subprogram.getLine(), 0};
}

bool scalarIntegerSourceType(const llvm::DIType* type, unsigned depth = 0) {
    if (!type || depth > 8) return false;
    if (const auto* basic = llvm::dyn_cast<llvm::DIBasicType>(type)) {
        switch (basic->getEncoding()) {
        case llvm::dwarf::DW_ATE_boolean: case llvm::dwarf::DW_ATE_signed:
        case llvm::dwarf::DW_ATE_unsigned: case llvm::dwarf::DW_ATE_signed_char:
        case llvm::dwarf::DW_ATE_unsigned_char: return true;
        default: return false;
        }
    }
    if (const auto* derived = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
        switch (derived->getTag()) {
        case llvm::dwarf::DW_TAG_typedef: case llvm::dwarf::DW_TAG_const_type:
        case llvm::dwarf::DW_TAG_volatile_type: case llvm::dwarf::DW_TAG_restrict_type:
            return scalarIntegerSourceType(derived->getBaseType(), depth + 1);
        default: return false;
        }
    }
    if (const auto* composite = llvm::dyn_cast<llvm::DICompositeType>(type)) {
        return composite->getTag() == llvm::dwarf::DW_TAG_enumeration_type &&
               scalarIntegerSourceType(composite->getBaseType(), depth + 1);
    }
    return false;
}

struct Evidence {
    const llvm::DILocalVariable* variable = nullptr;
    std::set<unsigned> arguments;
    bool expression = false;
    bool fragment = false;
    bool address = false;
    bool seen = false;
};

std::string missingFunctionReason(llvm::Module& module, const std::string& name) {
    for (auto& function : module) {
        for (auto& block : function) {
            for (auto& instruction : block) {
                const auto& location = instruction.getDebugLoc();
                if (!location || !location->getInlinedAt()) continue;
                const auto* subprogram = location->getScope()->getSubprogram();
                if (subprogram && (subprogram->getName() == name || subprogram->getLinkageName() == name)) {
                    return "[inline-evidence] function '" + name + "' is absent as a selectable LLVM function; "
                           "instruction debug locations record its inlining into " + functionName(function) +
                           ". Reconstructing eliminated source parameters is unsupported.";
                }
            }
        }
    }
    return "[function-not-found] '" + name + "' is absent from this module. " +
           (module.getNamedMetadata("llvm.dbg.cu") ? std::string("Debug metadata is present. ") :
                                                    std::string("Compile-unit debug metadata is missing. ")) +
           "Optimization/inlining, a different name, or a missing compilation unit may be involved; "
           "the cause is not established by available evidence.";
}

} // namespace

std::string functionName(const llvm::Function& function) {
    auto symbol = function.getName().str();
    if (const auto* original = function.getMetadata("valuetrace.original"))
        if (original->getNumOperands() == 1)
            if (const auto* name = llvm::dyn_cast<llvm::MDString>(original->getOperand(0))) symbol = name->getString().str();
    if (symbol.starts_with('?')) {
        const auto flags = static_cast<llvm::MSDemangleFlags>(llvm::MSDF_NoAccessSpecifier |
            llvm::MSDF_NoCallingConvention | llvm::MSDF_NoReturnType | llvm::MSDF_NoMemberType);
        if (char* text = llvm::microsoftDemangle(symbol, nullptr, nullptr, flags)) {
            std::string result(text);
            std::free(text);
            return result;
        }
    }
    return llvm::demangle(symbol);
}

std::vector<llvm::Function*> matchFunctions(llvm::Module& module, const std::string& name) {
    if (auto* exact = module.getFunction(name)) return {exact};
    std::vector<llvm::Function*> matches;
    for (auto& function : module) {
        const auto signature = functionName(function);
        if (signature == name || signature.substr(0, signature.find('(')) == name ||
            (function.getSubprogram() && function.getSubprogram()->getName() == name)) matches.push_back(&function);
    }
    if (matches.empty()) throw std::invalid_argument(missingFunctionReason(module, name));
    std::sort(matches.begin(), matches.end(), [](const auto* a, const auto* b) { return a->getName() < b->getName(); });
    return matches;
}

llvm::Function& resolveFunction(llvm::Module& module, const std::string& name) {
    const auto matches = matchFunctions(module, name);
    if (matches.size() != 1) {
        std::string message = "[ambiguous-function] choose one overload of '" + name + "':";
        for (auto* match : matches) message += "\n  " + functionName(*match) + " [" + match->getName().str() + "]";
        throw std::invalid_argument(message);
    }
    return *matches.front();
}

FunctionCandidate describeFunction(llvm::Function& function) {
    FunctionCandidate result;
    result.symbol = function.getName().str();
    if (const auto* provenance = function.getMetadata("valuetrace.input")) {
        if (provenance->getNumOperands() == 1)
            if (const auto* file = llvm::dyn_cast<llvm::MDString>(provenance->getOperand(0))) result.irInput = file->getString().str();
    }
    result.signature = functionName(function);
    for (const auto& argument : function.args()) {
        if (argument.getType()->isIntegerTy() || argument.getType()->isPointerTy())
            result.scalarIntegerArguments.push_back(argument.getArgNo());
    }
    const auto* subprogram = function.getSubprogram();
    result.debugInfoAvailable = subprogram != nullptr;
    if (subprogram) {
        result.source = sourceLocation(*subprogram);
        if (subprogram->isOptimized()) result.notes.push_back("Function is marked optimized; this alone does not establish why a parameter mapping is missing.");
    } else {
        result.notes.push_back("[debug-info-missing] No function debug metadata; source parameter names cannot be recovered reliably.");
    }

    std::map<const llvm::DILocalVariable*, Evidence> variables;
    const auto isOwnParameter = [&](const llvm::DILocalVariable* variable) {
        return subprogram && variable && variable->isParameter() && variable->getScope()->getSubprogram() == subprogram;
    };
    if (subprogram) {
        for (auto* metadata : subprogram->getRetainedNodes()) {
            if (const auto* variable = llvm::dyn_cast<llvm::DILocalVariable>(metadata); isOwnParameter(variable)) {
                variables[variable].variable = variable;
            }
        }
    }
    const auto observe = [&](const llvm::DILocalVariable* variable, const llvm::DIExpression* expression,
                             bool directValue, const std::vector<llvm::Value*>& values,
                             const llvm::DILocation* debugLocation) {
        if (!isOwnParameter(variable) || (debugLocation && debugLocation->getInlinedAt())) return;
        auto& evidence = variables[variable];
        evidence.variable = variable;
        evidence.seen = true;
        evidence.fragment |= expression && expression->isFragment();
        evidence.address |= !directValue;
        if (!directValue || !expression || expression->getNumElements() != 0 || values.size() != 1) {
            evidence.expression = true;
            return;
        }
        if (const auto* argument = llvm::dyn_cast<llvm::Argument>(values.front()); argument && argument->getParent() == &function) {
            evidence.arguments.insert(argument->getArgNo());
        }
    };
    bool hasInlineRecords = false;
    for (auto& block : function) {
        for (auto& instruction : block) {
            const auto& location = instruction.getDebugLoc();
            hasInlineRecords |= location && location->getInlinedAt();
            for (auto& record : instruction.getDbgRecordRange()) {
                if (const auto* variable = llvm::dyn_cast<llvm::DbgVariableRecord>(&record)) {
                    std::vector<llvm::Value*> values;
                    for (auto* value : variable->location_ops()) values.push_back(value);
                    observe(variable->getVariable(), variable->getExpression(), variable->isDbgValue(), values, variable->getDebugLoc().get());
                }
            }
            if (const auto* variable = llvm::dyn_cast<llvm::DbgVariableIntrinsic>(&instruction)) {
                std::vector<llvm::Value*> values;
                for (auto* value : variable->location_ops()) values.push_back(value);
                observe(variable->getVariable(), variable->getExpression(), llvm::isa<llvm::DbgValueInst>(variable), values, location.get());
            }
        }
    }
    if (hasInlineRecords) result.notes.push_back("[inline-evidence] Inlined instruction locations are present; source-level call-chain reconstruction is unsupported.");

    std::vector<Evidence> ordered;
    for (const auto& [variable, evidence] : variables) ordered.push_back(evidence);
    std::sort(ordered.begin(), ordered.end(), [](const auto& a, const auto& b) {
        if (a.variable->getArg() != b.variable->getArg()) return a.variable->getArg() < b.variable->getArg();
        return a.variable->getName() < b.variable->getName();
    });
    std::set<unsigned> represented;
    for (const auto& evidence : ordered) {
        ParameterCandidate parameter;
        parameter.name = evidence.variable->getName().str();
        parameter.sourceType = sourceType(evidence.variable->getType());
        parameter.implicit = evidence.variable->isArtificial() || parameter.name == "this";
        for (auto index : evidence.arguments) represented.insert(index);
        if (evidence.fragment) {
            parameter.status = "fragmented-mapping";
            parameter.detail = "Debug expression describes only part of the source parameter; aggregate/split mappings are unsupported.";
        } else if (evidence.arguments.size() > 1) {
            parameter.status = "ambiguous-mapping";
            parameter.detail = "Debug locations refer to multiple LLVM arguments:";
            for (auto index : evidence.arguments) parameter.detail += " " + std::to_string(index);
            parameter.detail += "; no argument was guessed.";
        } else if (evidence.arguments.empty()) {
            parameter.status = evidence.address ? "address-mapping-unsupported" :
                               evidence.expression ? "transformed-mapping" :
                               evidence.seen ? "non-argument-location" : "mapping-missing";
            parameter.detail = evidence.address ? "Debug metadata describes a memory address; direct LLVM argument mapping is not established." :
                               evidence.expression ? "Debug location requires an expression; an identity mapping to an LLVM argument is not established." :
                               evidence.seen ? "Debug locations are constants, computed values or unavailable locations, not direct LLVM arguments." :
                                               "Source parameter metadata exists, but no usable location record was retained.";
            parameter.detail += " Optimization may be involved; the specific cause is not established.";
        } else {
            parameter.llvmIndex = *evidence.arguments.begin();
            const auto* argument = function.getArg(*parameter.llvmIndex);
            parameter.irType = typeText(*argument->getType());
            parameter.traceable = (argument->getType()->isIntegerTy() && scalarIntegerSourceType(evidence.variable->getType())) ||
                                  (argument->getType()->isPointerTy() && !parameter.implicit &&
                                   (parameter.sourceType.find('*') != std::string::npos ||
                                    parameter.sourceType.find('&') != std::string::npos));
            parameter.status = parameter.traceable ? "mapped" :
                               argument->getType()->isIntegerTy() ? "unsupported-source-type" : "unsupported-type";
            parameter.detail = parameter.traceable ? "Identity debug value directly references LLVM argument " + std::to_string(*parameter.llvmIndex) + "." :
                "Mapped to LLVM argument " + std::to_string(*parameter.llvmIndex) + "; only scalar integer source parameters are traceable (pointer/this, aggregate or unknown source types are unsupported).";
        }
        result.parameters.push_back(std::move(parameter));
    }
    for (const auto& argument : function.args()) {
        if (represented.contains(argument.getArgNo())) continue;
        ParameterCandidate parameter;
        parameter.llvmIndex = argument.getArgNo();
        parameter.irType = typeText(*argument.getType());
        parameter.sourceType = "<unavailable>";
        parameter.traceable = argument.getType()->isIntegerTy() || argument.getType()->isPointerTy();
        parameter.implicit = argument.hasStructRetAttr();
        parameter.status = subprogram ? "source-name-unavailable" : "debug-info-missing";
        parameter.detail = "No reliable source-name mapping for LLVM argument " + std::to_string(argument.getArgNo()) + ".";
        if (parameter.implicit) parameter.detail += " Compiler return-storage parameter (sret).";
        if (!parameter.traceable) parameter.detail += " Non-integer type is not traceable.";
        result.parameters.push_back(std::move(parameter));
    }
    return result;
}

std::vector<unsigned> resolveParameters(const FunctionCandidate& function, const std::vector<unsigned>& indices,
                                      const std::vector<std::string>& names, std::vector<std::string>& messages) {
    std::set<unsigned> selected;
    for (auto index : indices) {
        if (std::find(function.scalarIntegerArguments.begin(), function.scalarIntegerArguments.end(), index) == function.scalarIntegerArguments.end()) {
            throw std::invalid_argument("[invalid-parameter] LLVM argument " + std::to_string(index) + " is absent or not a supported scalar integer.");
        }
        selected.insert(index);
        messages.push_back("LLVM argument " + std::to_string(index) + " selected explicitly by index; no source-name mapping is asserted.");
    }
    for (const auto& name : names) {
        if (name.empty()) throw std::invalid_argument("[invalid-name] parameter name must not be empty");
        std::vector<const ParameterCandidate*> matches;
        for (const auto& parameter : function.parameters) if (parameter.name == name) matches.push_back(&parameter);
        if (matches.empty()) {
            const auto status = function.debugInfoAvailable ? "parameter-name-not-found" : "debug-info-missing";
            throw std::invalid_argument("[" + std::string(status) + "] source parameter '" + name + "' cannot be matched in " + function.signature +
                ". No source-name guess or LLVM SSA-name fallback was used. Inspect candidates; optimization/inlining is not established as the cause.");
        }
        if (matches.size() != 1) throw std::invalid_argument("[ambiguous-parameter] multiple source parameter records named '" + name + "'; inspect and choose a candidate explicitly.");
        const auto& parameter = *matches.front();
        if (!parameter.traceable || !parameter.llvmIndex) throw std::invalid_argument("[" + parameter.status + "] '" + name + "': " + parameter.detail);
        selected.insert(*parameter.llvmIndex);
        messages.push_back("Source parameter '" + name + "' -> LLVM argument " + std::to_string(*parameter.llvmIndex) + " (" + parameter.irType + "); " + parameter.detail);
    }
    if (selected.empty()) throw std::invalid_argument("[selection-required] select a source parameter name or LLVM argument index");
    return {selected.begin(), selected.end()};
}

} // namespace valuetrace::selection

namespace valuetrace {

std::vector<FunctionCandidate> inspectParameters(const std::filesystem::path& file, const std::string& function) {
    return inspectParameters(std::vector<std::filesystem::path>{file}, function);
}
std::vector<FunctionCandidate> inspectParameters(const std::vector<std::filesystem::path>& files, const std::string& function) {
    llvm::LLVMContext context;
    auto module = selection::loadModule(files, context);
    std::vector<FunctionCandidate> result;
    for (auto* candidate : selection::matchFunctions(*module, function)) result.push_back(selection::describeFunction(*candidate));
    return result;
}

} // namespace valuetrace
