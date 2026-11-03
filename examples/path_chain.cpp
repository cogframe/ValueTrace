static_assert(sizeof(unsigned int) == 4);
static_assert(sizeof(unsigned long long) == 8);

extern "C" { volatile unsigned long long observedLength = 0; }

extern "C" [[gnu::noinline]] unsigned long long C(unsigned long long length) {
    // Preserve this observable call even under interprocedural optimization;
    // the replay checks the argument actually received at the target boundary.
    observedLength = length;
    return length;
}

extern "C" [[gnu::noinline]] unsigned long long B(unsigned int count) {
    return C(static_cast<unsigned long long>(count) * 4ull);
}

extern "C" [[gnu::noinline]] unsigned long long A(unsigned int input) {
#if GUARDED
    if (input > 1024u) return 0;
#endif
    return B(input);
}
