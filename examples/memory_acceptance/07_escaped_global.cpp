#include <cstdint>

static std::uint32_t* exported;
extern "C" void UnknownExternal(std::uint32_t*);

extern "C" __attribute__((noinline)) void RelayEscaped() {
    UnknownExternal(exported);
}

extern "C" std::uint32_t EscapeGlobal(std::uint32_t value) {
    std::uint32_t local = value;
    exported = &local;
    RelayEscaped();
    return local + 1u;
}
