#include "chain.h"
extern "C" unsigned CrossA(unsigned value) {
#if GUARDED
    if (value > 1000u) return 0;
#endif
    return CrossB(value);
}
static unsigned localStep(unsigned value) { return value + 1u; }
extern "C" unsigned StaticA(unsigned value) { return localStep(value); }
extern "C" unsigned CrossMissing(unsigned value);
extern "C" unsigned CrossAfter(unsigned value) { return CrossMissing(value) + value; }
