/*
  HLK-LD2402 GPIO-only presence example for a classic Arduino UNO.

  Wiring:
    LD2402 J2 pin 2 OT/IO -> UNO D2.
    LD2402 J2 pin 3 GND -> UNO GND.

  The OT/IO output is 0-3.3 V. The sensor drives the input, so do not enable
  a pull-up or pull-down resistor. Check the UNO input thresholds before use.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint8_t SENSOR_IO_PIN = 2;  // UNO D2
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t DEBOUNCE_TIME_MS = 30UL;

LD2402PresencePin presencePin(SENSOR_IO_PIN, DEBOUNCE_TIME_MS);

void emitPresenceState() {
  Serial.print(F("io_raw:"));
  Serial.print(presencePin.rawPresence() ? 1 : 0);
  Serial.print(F(" presence:"));
  Serial.println(presencePin.isPresent() ? 1 : 0);
}

void updateStatusLed() {
  digitalWrite(LED_BUILTIN, presencePin.isPresent() ? HIGH : LOW);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);
  presencePin.begin();
  updateStatusLed();
  emitPresenceState();
}

void loop() {
  if (presencePin.update()) {
    updateStatusLed();
    emitPresenceState();
  }
}
