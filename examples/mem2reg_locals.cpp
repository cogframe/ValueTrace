#include <cstdint>

extern "C" std::uint32_t LocalSteps(std::uint32_t input) {
    std::uint32_t copy = input;
    std::uint32_t scaled = copy * 4u;
    std::uint32_t result = scaled + 8u;
    return result;
}

extern "C" std::uint32_t LocalBranch(std::uint32_t input) {
    std::uint32_t copy = input;
    std::uint32_t result = 0;
    if (copy < 1000u) {
        result = copy + 8u;
    } else {
        result = copy * 2u;
    }
    return result;
}

extern "C" std::uint32_t LocalNarrow(std::uint64_t input) {
    std::uint64_t copy = input;
    std::uint64_t adjusted = copy + 8u;
    std::uint32_t narrowed = static_cast<std::uint32_t>(adjusted);
    return narrowed;
}
