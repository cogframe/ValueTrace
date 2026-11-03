#pragma once
#include <filesystem>
#include <vector>

namespace valuetrace {
// Expand immediate .ll/.bc directory entries, canonicalize and sort. Repeated
// files are errors; subdirectories are not searched implicitly.
std::vector<std::filesystem::path> resolveProgramInputs(const std::vector<std::filesystem::path>& inputs);
}
