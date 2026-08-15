/*
  Raw LD2402 UART monitor for a classic Arduino UNO.

  Wiring: sensor T/TX -> D10, R/RX -> D11 through a level shifter, GND -> UNO GND.
  SoftwareSerial at 115200 baud may be unreliable.
*/

#include <Arduino.h>
#include <SoftwareSerial.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint8_t SENSOR_RX_PIN = 10;
constexpr uint8_t SENSOR_TX_PIN = 11;

SoftwareSerial sensorSerial(SENSOR_RX_PIN, SENSOR_TX_PIN);

void setup() {
  Serial.begin(MONITOR_BAUD);
  sensorSerial.begin(SENSOR_BAUD);
}

void loop() {
  while (sensorSerial.available() > 0) {
    const int value = sensorSerial.read();
    if (value >= 0) {
      Serial.write(static_cast<uint8_t>(value));
    }
  }
}
