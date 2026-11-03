#include <cstdint>
struct Context { std::uint32_t len; std::uint32_t count; };
extern "C" __attribute__((noinline)) void ChangeCount(Context* ctx, std::uint32_t value) {
    ctx->count = value;
}
extern "C" __attribute__((noinline)) std::uint32_t UseLen(const Context* ctx) {
    return ctx->len + 8u;
}
extern "C" std::uint32_t FieldCheck(std::uint32_t count) {
    Context ctx{100, 0};
    ChangeCount(&ctx, count);
    return UseLen(&ctx);
}
