#pragma once
#include <stdint.h>
#define LOW 0
#define HIGH 1
#define OUTPUT 1
extern uint32_t fake_now;
extern int pin_value;
inline uint32_t millis() { return fake_now; }
inline void digitalWrite(int pin, int value) { if (pin != 47) __builtin_trap(); pin_value = value; }
inline void pinMode(int pin, int mode) { if (pin != 47 || mode != OUTPUT) __builtin_trap(); }
