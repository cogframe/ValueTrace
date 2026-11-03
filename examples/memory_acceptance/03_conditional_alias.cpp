#include <cstdint>
struct Context { std::uint32_t len; std::uint32_t count; };
extern "C" __attribute__((noinline)) void ConditionalWrite(Context* first, Context* second,
                                                              bool chooseFirst, std::uint32_t value) {
    Context* target = chooseFirst ? first : second;
    target->len = value;
}
extern "C" std::uint32_t ConditionalRead(bool chooseFirst, std::uint32_t value) {
    Context first{100, 0};
    Context second{200, 0};
    ConditionalWrite(&first, &second, chooseFirst, value);
    return first.len + 8u;
}
