/*
  HLK-LD2402 GPIO-only presence example for Arduino UNO R4.

  Wiring:
    LD2402 J2 pin 2 OT/IO -> UNO R4 D2.
    LD2402 J2 pin 3 GND -> UNO R4 GND.

  The OT/IO signal is 3.3 V and is driven by the sensor. Do not enable a
  pull-up or pull-down resistor in the sketch.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint8_t SENSOR_IO_PIN = 2;  // UNO R4 D2
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
