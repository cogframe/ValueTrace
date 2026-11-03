#pragma once
extern "C" unsigned CrossA(unsigned value);
extern "C" unsigned CrossB(unsigned value);
extern "C" unsigned CrossC(unsigned length);
extern "C" unsigned StaticA(unsigned value);
extern "C" unsigned StaticB(unsigned value);
extern volatile unsigned observedCrossLength;
