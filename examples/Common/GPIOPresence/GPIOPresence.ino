/*
  HLK-LD2402 GPIO-only presence example.

  Manual reference: HLK-LD2402 V1.09, section 3, J2 pin table (PDF page 6 /
  printed manual page 3): J2 pin 2 IO is high for a detected person and low
  otherwise, with a 0-3.3 V signal range.

  Wiring:
    LD2402 J2 pin 2 IO -> SENSOR_IO_PIN below (Pico W GP2 by default).
    LD2402 J2 pin 3 G  -> board GND.
    Use an input level shifter if the selected board requires one; do not use
    INPUT_PULLUP or INPUT_PULLDOWN because the sensor drives this signal.
    On an UNO AVR, keep the 3.3 V signal within the board's input limits.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint8_t SENSOR_IO_PIN = 2;  // Pico W GP2; use D2/GPIO2 on others
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t DEBOUNCE_TIME_MS = 30UL;

LD2402PresencePin presencePin(SENSOR_IO_PIN, DEBOUNCE_TIME_MS);

void emitPresenceState() {
  Serial.print(F("io_raw:"));
  Serial.print(presencePin.rawPresence() ? 1 : 0);
  Serial.print(F(" presence:"));
  Serial.println(presencePin.isPresent() ? 1 : 0);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  presencePin.begin();

  // Emit the initial debounced state; subsequent lines are state changes.
  emitPresenceState();
}

void loop() {
  if (presencePin.update()) {
    emitPresenceState();
  }
}
