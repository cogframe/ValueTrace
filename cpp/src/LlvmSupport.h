#pragma once

#include "valuetrace/ParameterSelection.h"
#include <memory>

namespace llvm { class Module; class Function; class LLVMContext; }

namespace valuetrace::selection {

std::unique_ptr<llvm::Module> loadModule(const std::filesystem::path&, llvm::LLVMContext&);
std::unique_ptr<llvm::Module> loadModule(const std::vector<std::filesystem::path>&, llvm::LLVMContext&);
std::string functionName(const llvm::Function&);
std::vector<llvm::Function*> matchFunctions(llvm::Module&, const std::string&);
llvm::Function& resolveFunction(llvm::Module&, const std::string&);
FunctionCandidate describeFunction(llvm::Function&);
std::vector<unsigned> resolveParameters(const FunctionCandidate&, const std::vector<unsigned>&,
                                      const std::vector<std::string>&, std::vector<std::string>& messages);

} // namespace valuetrace::selection
