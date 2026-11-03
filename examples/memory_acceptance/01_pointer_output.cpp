#include <cstdint>
struct Context { std::uint32_t len; std::uint32_t count; };
extern "C" __attribute__((noinline)) void B(Context* ctx, std::uint32_t value) {
    ctx->len = value;
}
extern "C" __attribute__((noinline)) std::uint32_t C(const Context* ctx) {
    return ctx->len + 8u;
}
extern "C" std::uint32_t A(std::uint32_t value) {
    Context ctx{0, 0};
    B(&ctx, value);
    return C(&ctx);
}
