/*
  Raw LD2402 UART monitor for Arduino UNO R4.

  Wiring: sensor T/TX -> D0/Serial1 RX, R/RX -> D1/Serial1 TX, GND -> UNO R4 GND.
*/

#include <Arduino.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;

void setup() {
  Serial.begin(MONITOR_BAUD);
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
