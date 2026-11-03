#include "valuetrace/ProgramInput.h"
#include <algorithm>
#include <cctype>
#include <set>
#include <stdexcept>

namespace valuetrace {
namespace {
bool isIr(const std::filesystem::path& path) {
    auto extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return extension == ".ll" || extension == ".bc";
}
}
std::vector<std::filesystem::path> resolveProgramInputs(const std::vector<std::filesystem::path>& inputs) {
    if (inputs.empty()) throw std::invalid_argument("[input-empty] provide at least one LLVM file or directory");
    std::set<std::filesystem::path> files;
    const auto add = [&](const std::filesystem::path& file) {
        if (!std::filesystem::is_regular_file(file) || !isIr(file))
            throw std::invalid_argument("[invalid-input] expected a .ll/.bc regular file: " + file.string());
        const auto canonical = std::filesystem::canonical(file);
        for (const auto& previous : files) if (std::filesystem::equivalent(previous, canonical))
            throw std::invalid_argument("[duplicate-input] LLVM file provided more than once: " + canonical.string());
        files.insert(canonical);
    };
    for (const auto& input : inputs) {
        if (!std::filesystem::exists(input)) throw std::invalid_argument("[input-missing] " + input.string());
        if (!std::filesystem::is_directory(input)) { add(input); continue; }
        std::size_t count = 0;
        for (const auto& entry : std::filesystem::directory_iterator(input)) {
            if (entry.is_regular_file() && isIr(entry.path())) { add(entry.path()); ++count; }
        }
        if (count == 0) throw std::invalid_argument("[input-empty] directory contains no immediate .ll/.bc files: " + input.string());
    }
    return {files.begin(), files.end()};
}
}
