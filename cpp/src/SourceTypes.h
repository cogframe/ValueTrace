#pragma once

#include "valuetrace/DependencyGraph.h"
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <map>
#include <string>
#include <vector>

namespace valuetrace {

// Only identity debug-value locations are associated with SSA values. Complex
// expressions and fragments must not be used as signedness evidence.
class SourceTypes {
public:
    explicit SourceTypes(llvm::Function& function);
    IntegerTypeEvidence forValue(const llvm::Value& value,
                                 const OperationSemantics* operation = nullptr) const;

private:
    struct Observation {
        const llvm::DIType* type;
        std::string variable;
    };
    llvm::Function& function_;
    std::map<const llvm::Value*, std::vector<Observation>> direct_;
    std::map<const llvm::Value*, std::string> rejected_;
};

} // namespace valuetrace
