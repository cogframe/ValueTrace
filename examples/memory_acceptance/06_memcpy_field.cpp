#include <cstddef>
#include <cstdint>
struct Context { std::uint32_t len; std::uint32_t count; };
extern "C" void* memcpy(void*, const void*, std::size_t);
extern "C" __attribute__((noinline)) void CopyLen(Context* dst, const Context* src) {
    memcpy(&dst->len, &src->len, sizeof(dst->len));
}
extern "C" std::uint32_t MemcpyField(std::uint32_t value) {
    Context src{value, 1};
    Context dst{0, 2};
    CopyLen(&dst, &src);
    return dst.len + 8u;
}
