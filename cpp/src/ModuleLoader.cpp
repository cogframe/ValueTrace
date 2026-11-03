#include "LlvmSupport.h"
#include "valuetrace/ProgramInput.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/IR/DiagnosticPrinter.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>
#include <map>
#include <set>
#include <stdexcept>

namespace valuetrace::selection {
namespace {
struct Diagnostics {
    llvm::LLVMContext& context;
    llvm::DiagnosticHandler::DiagnosticHandlerTy previous;
    void* previousContext;
    std::string messages;
    bool failed = false;
    explicit Diagnostics(llvm::LLVMContext& ctx) : context(ctx), previous(ctx.getDiagnosticHandlerCallBack()), previousContext(ctx.getDiagnosticContext()) {
        context.setDiagnosticHandlerCallBack([](const llvm::DiagnosticInfo* info, void* opaque) {
            auto& self = *static_cast<Diagnostics*>(opaque);
            llvm::raw_string_ostream stream(self.messages);
            llvm::DiagnosticPrinterRawOStream printer(stream);
            info->print(printer); stream << '\n';
            self.failed |= info->getSeverity() == llvm::DS_Error || info->getSeverity() == llvm::DS_Warning;
        }, this);
    }
    ~Diagnostics() { context.setDiagnosticHandlerCallBack(previous, previousContext); }
};

bool compatible(llvm::Type* a, llvm::Type* b, std::set<std::pair<llvm::Type*, llvm::Type*>>& seen) {
    if (a == b) return true;
    if (a->getTypeID() != b->getTypeID()) return false;
    if (!seen.emplace(a, b).second) return true;
    if (auto* x = llvm::dyn_cast<llvm::IntegerType>(a)) return x->getBitWidth() == llvm::cast<llvm::IntegerType>(b)->getBitWidth();
    if (auto* x = llvm::dyn_cast<llvm::PointerType>(a)) return x->getAddressSpace() == llvm::cast<llvm::PointerType>(b)->getAddressSpace();
    if (auto* x = llvm::dyn_cast<llvm::ArrayType>(a)) {
        auto* y = llvm::cast<llvm::ArrayType>(b);
        return x->getNumElements() == y->getNumElements() && compatible(x->getElementType(), y->getElementType(), seen);
    }
    if (auto* x = llvm::dyn_cast<llvm::VectorType>(a)) {
        auto* y = llvm::cast<llvm::VectorType>(b);
        return x->getElementCount() == y->getElementCount() && compatible(x->getElementType(), y->getElementType(), seen);
    }
    if (auto* x = llvm::dyn_cast<llvm::StructType>(a)) {
        auto* y = llvm::cast<llvm::StructType>(b);
        if (x->isOpaque() || y->isOpaque()) return true; // A forward declaration can be completed by the linker.
        if (x->isPacked() != y->isPacked() || x->getNumElements() != y->getNumElements()) return false;
        for (unsigned i = 0; i < x->getNumElements(); ++i) if (!compatible(x->getElementType(i), y->getElementType(i), seen)) return false;
        return true;
    }
    if (auto* x = llvm::dyn_cast<llvm::FunctionType>(a)) {
        auto* y = llvm::cast<llvm::FunctionType>(b);
        if (x->isVarArg() != y->isVarArg() || x->getNumParams() != y->getNumParams() || !compatible(x->getReturnType(), y->getReturnType(), seen)) return false;
        for (unsigned i = 0; i < x->getNumParams(); ++i) if (!compatible(x->getParamType(i), y->getParamType(i), seen)) return false;
        return true;
    }
    return false; // Other canonical primitive types have already compared equal in this LLVMContext.
}
bool compatible(llvm::Type* a, llvm::Type* b) {
    std::set<std::pair<llvm::Type*, llvm::Type*>> seen;
    return compatible(a, b, seen);
}

bool abiCompatible(const llvm::Function& a, const llvm::Function& b) {
    if (a.getCallingConv() != b.getCallingConv()) return false;
    const auto check = [](llvm::AttributeSet x, llvm::AttributeSet y) {
        for (auto kind : {llvm::Attribute::SExt, llvm::Attribute::ZExt, llvm::Attribute::InReg,
                llvm::Attribute::StructRet, llvm::Attribute::ByVal, llvm::Attribute::InAlloca,
                llvm::Attribute::Preallocated, llvm::Attribute::Nest, llvm::Attribute::SwiftSelf,
                llvm::Attribute::SwiftError, llvm::Attribute::SwiftAsync})
            if (x.getAttribute(kind) != y.getAttribute(kind)) return false;
        return true;
    };
    if (!check(a.getAttributes().getRetAttrs(), b.getAttributes().getRetAttrs())) return false;
    for (unsigned i = 0; i < a.arg_size(); ++i)
        if (!check(a.getAttributes().getParamAttrs(i), b.getAttributes().getParamAttrs(i))) return false;
    return true;
}

void verify(llvm::Module& module, const std::string& label) {
    std::string text; llvm::raw_string_ostream stream(text);
    if (llvm::verifyModule(module, &stream)) throw std::runtime_error("[invalid-module] " + label + ": " + text);
}

void preflight(const std::vector<std::unique_ptr<llvm::Module>>& modules, const std::vector<std::filesystem::path>& files) {
    struct Symbol { const llvm::GlobalValue* value; std::size_t file; };
    std::map<std::string, std::vector<Symbol>> symbols;
    const auto& first = *modules.front();
    for (std::size_t index = 0; index < modules.size(); ++index) {
        const auto& module = *modules[index];
        if (module.getTargetTriple() != first.getTargetTriple() || module.getDataLayoutStr() != first.getDataLayoutStr())
            throw std::invalid_argument("[target-mismatch] target triple/data layout differ: " + files.front().string() + " vs " + files[index].string());
        for (const auto& value : module.global_values()) {
            if (value.hasLocalLinkage() || !value.hasName()) continue;
            auto& previous = symbols[value.getName().str()];
            for (const auto& other : previous) {
                const auto& before = *other.value;
                const auto conflict = [&](const char* code) {
                    throw std::invalid_argument(std::string(code) + " symbol '" + value.getName().str() + "': " + files[other.file].string() + " vs " + files[index].string());
                };
                if (before.getValueID() != value.getValueID() || before.getAddressSpace() != value.getAddressSpace()) conflict("[symbol-type-mismatch]");
                // Appending arrays (llvm.used, global_ctors, ...) are concatenated, not chosen definitions.
                if (before.hasAppendingLinkage() && value.hasAppendingLinkage()) {
                    auto* a = llvm::dyn_cast<llvm::ArrayType>(before.getValueType());
                    auto* b = llvm::dyn_cast<llvm::ArrayType>(value.getValueType());
                    if (!a || !b || !compatible(a->getElementType(), b->getElementType())) conflict("[symbol-type-mismatch]");
                    continue;
                }
                if (!compatible(before.getValueType(), value.getValueType())) conflict("[symbol-type-mismatch]");
                if (auto* a = llvm::dyn_cast<llvm::Function>(&before)) {
                    if (!abiCompatible(*a, llvm::cast<llvm::Function>(value))) conflict("[function-abi-mismatch]");
                }
                if (auto* a = llvm::dyn_cast<llvm::GlobalVariable>(&before)) {
                    const auto& b = llvm::cast<llvm::GlobalVariable>(value);
                    if (a->isConstant() != b.isConstant() || a->getThreadLocalMode() != b.getThreadLocalMode()) conflict("[global-properties-mismatch]");
                }
                if (!before.isDeclarationForLinker() && !value.isDeclarationForLinker()) conflict("[duplicate-definition]");
            }
            previous.push_back({&value, index});
        }
    }
}
}

std::unique_ptr<llvm::Module> loadModule(const std::filesystem::path& file, llvm::LLVMContext& context) {
    return loadModule(std::vector<std::filesystem::path>{file}, context);
}
std::unique_ptr<llvm::Module> loadModule(const std::vector<std::filesystem::path>& inputs, llvm::LLVMContext& context) {
    const auto files = resolveProgramInputs(inputs);
    Diagnostics diagnostics(context);
    std::vector<std::unique_ptr<llvm::Module>> modules;
    for (std::size_t index = 0; index < files.size(); ++index) {
        llvm::SMDiagnostic diagnostic;
        auto module = llvm::parseIRFile(files[index].string(), diagnostic, context);
        if (!module) {
            std::string text; llvm::raw_string_ostream stream(text); diagnostic.print("valuetrace", stream);
            throw std::runtime_error("[input-parse] " + text);
        }
        if (diagnostics.failed) throw std::runtime_error("[input-diagnostic] " + files[index].string() + ": " + diagnostics.messages);
        verify(*module, files[index].string());
        std::vector<llvm::GlobalValue*> retainedLocals;
        for (auto& function : *module) {
            if (function.isIntrinsic()) continue;
            function.setMetadata("valuetrace.input", llvm::MDNode::get(context, llvm::MDString::get(context, files[index].generic_string())));
            if (files.size() > 1 && function.hasLocalLinkage()) {
                const auto original = function.getName().str();
                function.setMetadata("valuetrace.original", llvm::MDNode::get(context, llvm::MDString::get(context, original)));
                function.setName(original + ".vt." + std::to_string(index));
                if (!function.isDeclaration()) retainedLocals.push_back(&function);
            }
        }
        // The ordinary linker can discard unreferenced source-local functions.
        // Retain these user-selectable roots without runtime instructions.
        if (!retainedLocals.empty()) llvm::appendToCompilerUsed(*module, retainedLocals);
        modules.push_back(std::move(module));
    }
    preflight(modules, files);
    auto combined = std::move(modules.front());
    llvm::Linker linker(*combined);
    for (std::size_t index = 1; index < modules.size(); ++index) {
        const bool failed = linker.linkInModule(std::move(modules[index]));
        if (failed || diagnostics.failed) throw std::runtime_error("[module-link] " + files[index].string() + ": " + diagnostics.messages);
    }
    verify(*combined, "linked input");
    return combined;
}
}
