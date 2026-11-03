#include <cstdint>
struct Context { std::uint32_t len; std::uint32_t count; };
extern "C" __attribute__((noinline)) void WriteUnknown(Context* target, std::uint32_t value) {
    target->len = value;
}
// target comes from outside the supplied program and has no known object identity.
extern "C" std::uint32_t ExternalPointer(Context* target, std::uint32_t value) {
    WriteUnknown(target, value);
    return target->len + 8u;
}
