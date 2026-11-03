#include <cstdint>
extern "C" __attribute__((noinline)) std::uint32_t ReadZero(const std::uint32_t* values) {
    return values[0];
}
extern "C" std::uint32_t DynamicIndex(std::uint32_t index, std::uint32_t value) {
    std::uint32_t values[2]{100, 200};
    if (index >= 2) return 0; // Keep all accesses within the array.
    values[index] = value;
    return ReadZero(values) + 8u;
}
