#pragma once

#include "valuetrace/LlvmReader.h"
#include "valuetrace/ParameterSelection.h"

#include <optional>

namespace valuetrace::cli {

void printFunctions(const std::vector<FunctionCandidate>& functions);
void printParameters(const FunctionCandidate& function);
const FunctionCandidate& chooseFunction(const std::vector<FunctionCandidate>& functions,
                                       std::optional<unsigned> choice);
void addParameterChoice(const FunctionCandidate& function, unsigned choice, ParameterQuery& query);
void chooseParameters(const FunctionCandidate& function, ParameterQuery& query);

} // namespace valuetrace::cli
