/*
  Raw LD2402 UART monitor for Raspberry Pi Pico W.

  Wiring: sensor T/TX -> GP1, R/RX -> GP0, GND -> Pico W GND.
  This copies the normal-mode stream directly; it does not parse it.
*/

#include <Arduino.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;

void setup() {
  Serial.begin(MONITOR_BAUD);
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(SENSOR_BAUD);
}

void loop() {
  while (Serial1.available() > 0) {
    const int value = Serial1.read();
    if (value >= 0) {
      Serial.write(static_cast<uint8_t>(value));
    }
  }
}
