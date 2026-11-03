#include "chain.h"
volatile unsigned observedCrossLength = 0;
extern "C" unsigned CrossC(unsigned length) {
    observedCrossLength = length;
    return length;
}
