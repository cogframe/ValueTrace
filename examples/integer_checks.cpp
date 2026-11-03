// Plain functions and explicit integer widths for IR import and witness replay.
static_assert(sizeof(unsigned) == 4 && sizeof(unsigned long long) == 8);
extern "C" unsigned short Narrow32(unsigned value) { return static_cast<unsigned short>(value); }
extern "C" unsigned Narrow64(unsigned long long value) { return static_cast<unsigned>(value); }
extern "C" unsigned short GuardNarrow32(unsigned value) {
    return value <= 65535u ? Narrow32(value) : 0;
}
extern "C" unsigned GuardNarrow64(unsigned long long value) {
    return value <= 0xffffffffULL ? Narrow64(value) : 0;
}
extern "C" unsigned Shift32(unsigned value, unsigned amount) { return value << amount; }
extern "C" unsigned long long Shift64(unsigned long long value, unsigned long long amount) { return value << amount; }
extern "C" unsigned GuardShift32(unsigned value, unsigned amount) {
    return amount < 32u ? Shift32(value, amount) : 0;
}
