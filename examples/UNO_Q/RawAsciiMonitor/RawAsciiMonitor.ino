/*
  Raw LD2402 UART monitor for Arduino UNO Q.

  Wiring: sensor T/TX -> the UNO Q Serial1 RX pin, R/RX -> Serial1 TX pin,
  GND -> UNO Q GND. Check the board package pin map before wiring.
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
