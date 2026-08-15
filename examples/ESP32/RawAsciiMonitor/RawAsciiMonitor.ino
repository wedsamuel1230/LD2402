/*
  Raw LD2402 UART monitor for ESP32.

  Wiring: sensor T/TX -> GPIO16, R/RX -> GPIO17, GND -> ESP32 GND.
*/

#include <Arduino.h>
#include <HardwareSerial.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr int SENSOR_RX_PIN = 16;
constexpr int SENSOR_TX_PIN = 17;

HardwareSerial sensorSerial(2);

void setup() {
  Serial.begin(MONITOR_BAUD);
  sensorSerial.begin(SENSOR_BAUD, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
}

void loop() {
  while (sensorSerial.available() > 0) {
    const int value = sensorSerial.read();
    if (value >= 0) {
      Serial.write(static_cast<uint8_t>(value));
    }
  }
}
