// No platform headers: int is 32 bits and long long is 64 bits on this target.
static_assert(sizeof(unsigned int) == 4);
static_assert(sizeof(unsigned long long) == 8);

[[gnu::noinline]] unsigned long long C(unsigned long long length) {
    return length + 7ull;
}

[[gnu::noinline]] unsigned long long B(unsigned int count, unsigned int extra) {
    const unsigned int bytes = count * 4u + extra;
    const auto length = static_cast<unsigned short>(bytes);
    return C(static_cast<unsigned long long>(length));
}

[[gnu::noinline]] unsigned long long A(unsigned int input) {
    return B(input, 3u) + 1ull;
}
