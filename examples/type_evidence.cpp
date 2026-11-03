#include <cstddef>
#include <cstdint>

using CountBase = std::uint32_t;
using CountAlias = CountBase;

extern "C" std::uint32_t unsigned_sub(std::uint32_t x, std::uint32_t y) {
    std::uint32_t result = x - y;
    return result;
}

extern "C" std::uint32_t direct_return_sub(std::uint32_t x, std::uint32_t y) {
    return x - y;
}

extern "C" std::int32_t signed_sub(std::int32_t x, std::int32_t y) {
    std::int32_t result = x - y;
    return result;
}

extern "C" std::uint32_t mixed_signed_result(std::int32_t x, std::int32_t y) {
    std::uint32_t result = static_cast<std::uint32_t>(x - y);
    return result;
}

extern "C" CountAlias alias_sub(CountAlias x, CountAlias y) {
    CountAlias result = x - y;
    return result;
}

extern "C" std::size_t size_sub(std::size_t x, std::size_t y) {
    std::size_t result = x - y;
    return result;
}

extern "C" int char_value(char x) {
    return x;
}

extern "C" int unsigned_char_value(unsigned char x) {
    return x;
}

extern "C" bool bool_value(bool x) {
    return x;
}

extern "C" int promoted_u16_sum(std::uint16_t a, std::uint16_t b) {
    int result = a + b;
    return result;
}

extern "C" std::uint8_t narrowed_u8_sum(std::uint8_t a, std::uint8_t b) {
    std::uint8_t result = a + b;
    return result;
}

extern "C" int promoted_char_sum(char a, char b) {
    int result = a + b;
    return result;
}

extern "C" int promoted_bool_sum(bool a, bool b) {
    int result = a + b;
    return result;
}

enum class Mode : unsigned { First, Second };
extern "C" unsigned enum_value(Mode x) {
    return static_cast<unsigned>(x);
}

struct Bits { unsigned amount : 5; };
extern "C" unsigned bitfield_value(const Bits& bits) {
    return bits.amount;
}
