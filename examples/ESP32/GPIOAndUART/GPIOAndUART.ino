/*
  Compare LD2402 UART presence with its OT/IO signal on ESP32.

  Wiring: T/TX -> GPIO16, R/RX -> GPIO17, OT/IO -> GPIO4, GND -> ESP32 GND.
  GPIO4 is used so GPIO2 remains available for the common built-in LED.
*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t PRINT_INTERVAL_MS = 250UL;
constexpr uint32_t DATA_MAX_AGE_MS = 1000UL;
constexpr int SENSOR_RX_PIN = 16;
constexpr int SENSOR_TX_PIN = 17;
constexpr uint8_t SENSOR_IO_PIN = 4;

HardwareSerial sensorSerial(2);
LD2402 sensor(sensorSerial);
LD2402PresencePin presencePin(SENSOR_IO_PIN, 30UL);

void printComparison() {
  Serial.print(F("uart_distance_cm:"));
  Serial.print(sensor.getDistanceFilteredCm());
  Serial.print(F(" uart_presence:"));
  Serial.print(sensor.isPresent() ? 1 : 0);
  Serial.print(F(" gpio_presence:"));
  Serial.print(presencePin.isPresent() ? 1 : 0);
  Serial.print(F(" gpio_raw:"));
  Serial.print(presencePin.rawPresence() ? 1 : 0);
  Serial.print(F(" data_valid:"));
  Serial.println(sensor.isDataValid(DATA_MAX_AGE_MS) ? 1 : 0);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  sensorSerial.begin(SENSOR_BAUD, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
  presencePin.begin();
}

void loop() {
  sensor.update();
  presencePin.update();

  static uint32_t lastPrintMs = 0;
  const uint32_t now = millis();
  if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
    lastPrintMs = now;
    printComparison();
  }
}
