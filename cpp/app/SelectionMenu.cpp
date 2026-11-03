#include "SelectionMenu.h"

#include <charconv>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace valuetrace::cli {
namespace {

unsigned choiceNumber(std::string text) {
    const auto first = text.find_first_not_of(" \t\r");
    const auto last = text.find_last_not_of(" \t\r");
    if (first == std::string::npos) throw std::invalid_argument("[invalid-choice] empty choice");
    text = text.substr(first, last - first + 1);
    unsigned value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size() || value == 0) {
        throw std::invalid_argument("[invalid-choice] candidate numbers start at 1");
    }
    return value;
}

std::string readChoice(const std::string& description) {
    std::cout << description << std::flush;
    std::string text;
    if (!std::getline(std::cin, text)) {
        throw std::invalid_argument("[selection-required] no choice received; rerun with --function-choice/--param-choice, an exact function symbol, or --param-name");
    }
    return text;
}

} // namespace

void printFunctions(const std::vector<FunctionCandidate>& functions) {
    std::cout << "Function candidates:\n";
    for (std::size_t i = 0; i < functions.size(); ++i) {
        const auto& function = functions[i];
        std::cout << "  " << i + 1 << ". " << function.signature << " symbol=" << function.symbol;
        if (!function.source.file.empty()) std::cout << " source=" << function.source.file << ':' << function.source.line;
        else std::cout << " source=<debug location unavailable>";
        if (!function.irInput.empty()) std::cout << " input=" << function.irInput;
        std::cout << '\n';
    }
}

void printParameters(const FunctionCandidate& function) {
    std::cout << "Parameters of " << function.signature << ":\n";
    for (std::size_t i = 0; i < function.parameters.size(); ++i) {
        const auto& parameter = function.parameters[i];
        std::cout << "  " << i + 1 << ". " << (parameter.name.empty() ? "<source name unavailable>" : parameter.name)
                  << " source-type=" << parameter.sourceType;
        if (parameter.implicit) std::cout << " implicit";
        if (parameter.name == "this") std::cout << " (current object)";
        if (parameter.llvmIndex) std::cout << " -> LLVM arg" << *parameter.llvmIndex << ' ' << parameter.irType;
        std::cout << (parameter.traceable ? " [traceable]" : " [not traceable]")
                  << " [" << parameter.status << "] " << parameter.detail << '\n';
    }
    for (const auto& note : function.notes) std::cout << "  Note: " << note << '\n';
}

const FunctionCandidate& chooseFunction(const std::vector<FunctionCandidate>& functions,
                                       std::optional<unsigned> choice) {
    if (functions.size() > 1 || choice) printFunctions(functions);
    if (!choice && functions.size() == 1) return functions.front();
    const auto number = choice ? *choice : choiceNumber(readChoice("Choose function number: "));
    if (number == 0 || number > functions.size()) throw std::invalid_argument("[invalid-choice] function candidate out of range");
    return functions[number - 1];
}

void addParameterChoice(const FunctionCandidate& function, unsigned choice, ParameterQuery& query) {
    if (choice == 0 || choice > function.parameters.size()) throw std::invalid_argument("[invalid-choice] parameter candidate out of range");
    const auto& parameter = function.parameters[choice - 1];
    if (!parameter.traceable || !parameter.llvmIndex) {
        throw std::invalid_argument("[" + parameter.status + "] selected parameter '" + parameter.name + "': " + parameter.detail);
    }
    query.parameters.push_back(*parameter.llvmIndex);
    std::cout << "Chosen parameter: " << (parameter.name.empty() ? "<source name unavailable>" : parameter.name)
              << " -> LLVM argument " << *parameter.llvmIndex << '\n';
}

void chooseParameters(const FunctionCandidate& function, ParameterQuery& query) {
    if (function.parameters.empty()) throw std::invalid_argument("[no-parameters] no source or LLVM parameters are available");
    const auto text = readChoice("Choose parameter numbers (comma-separated): ");
    std::istringstream stream(text);
    std::string part;
    while (std::getline(stream, part, ',')) addParameterChoice(function, choiceNumber(part), query);
    if (query.parameters.empty() || (!text.empty() && text.back() == ',')) throw std::invalid_argument("[invalid-choice] empty parameter choice");
}

} // namespace valuetrace::cli
