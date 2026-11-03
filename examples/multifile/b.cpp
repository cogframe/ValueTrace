#include "chain.h"
extern "C" unsigned CrossB(unsigned value) { return CrossC(value + 8u); }
static unsigned localStep(unsigned value) { return value + 2u; }
extern "C" unsigned StaticB(unsigned value) { return localStep(value); }
