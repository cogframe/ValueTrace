#include <stdint.h>

extern "C" void consume(uint16_t length);

extern "C" __attribute__((noinline)) void convert(uint32_t count) {
    uint16_t length = count * 4;
    consume(length);
}

extern "C" __attribute__((noinline)) void source(uint32_t input) {
    convert(input);
}
