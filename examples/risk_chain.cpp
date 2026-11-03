// Unsigned arithmetic is defined even when it wraps; replay is safe.
volatile unsigned observedRiskLength = 0;
extern "C" unsigned RiskC(unsigned length) {
    observedRiskLength = length;
    return length;
}
extern "C" unsigned RiskB(unsigned count) {
    return RiskC(count + 8u);
}
extern "C" unsigned RiskA(unsigned count) {
#if GUARDED
    if (count > 1000u) return 0;
#endif
    return RiskB(count);
}
