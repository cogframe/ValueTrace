// Equivalent single-translation-unit reference for CrossA -> CrossB -> CrossC.
#include "chain.h"
volatile unsigned observedCrossLength = 0;
extern "C" unsigned CrossC(unsigned length) { observedCrossLength = length; return length; }
extern "C" unsigned CrossB(unsigned value) { return CrossC(value + 8u); }
extern "C" unsigned CrossA(unsigned value) {
#if GUARDED
    if (value > 1000u) return 0;
#endif
    return CrossB(value);
}
