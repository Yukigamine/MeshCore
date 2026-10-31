#pragma once

#include <Arduino.h>

#if defined(PIN_GPS_RX) && PIN_GPS_RX == DM_VIBRATION_PIN
#error "GPS RX and vibration must use different pins"
#endif
#if defined(PIN_GPS_TX) && PIN_GPS_TX == DM_VIBRATION_PIN
#error "GPS TX and vibration must use different pins"
#endif

// Active-high, non-blocking one/two-pulse notification. Bursts coalesce rather
// than leaving a long backlog of motor activations.
class DirectMessageVibration {
  bool active = false;
  uint32_t started = 0;
  uint8_t pulses = 2;

public:
  void begin() {
    digitalWrite(DM_VIBRATION_PIN, LOW);
    pinMode(DM_VIBRATION_PIN, OUTPUT);
  }

  void trigger(uint8_t pulse_count = 2) {
    // A DM upgrades an active channel alert; a channel never shortens a DM.
    if (active) {
      if (pulse_count == 2) pulses = 2;
      return;
    }
    pulses = pulse_count == 1 ? 1 : 2;
    started = millis();
    active = true;
    digitalWrite(DM_VIBRATION_PIN, HIGH);
  }

  void stop() {
    active = false;
    digitalWrite(DM_VIBRATION_PIN, LOW);
  }

  void loop() {
    if (!active) return;
    const uint32_t elapsed = uint32_t(millis()) - started;
    if (elapsed >= (pulses == 1 ? 200u : 600u)) {
      stop();
    } else {
      // 200 ms on, 200 ms off, 200 ms on.
      digitalWrite(DM_VIBRATION_PIN, elapsed < 200 || elapsed >= 400 ? HIGH : LOW);
    }
  }
};
