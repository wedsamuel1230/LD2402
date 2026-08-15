/*
  Compare LD2402 UART presence with its OT/IO signal on a classic Arduino UNO.

  Wiring: T/TX -> D10, R/RX -> D11 through a level shifter, OT/IO -> D2,
  GND -> UNO GND. SoftwareSerial at 115200 baud may be unreliable.
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t PRINT_INTERVAL_MS = 250UL;
constexpr uint32_t DATA_MAX_AGE_MS = 1000UL;
constexpr uint8_t SENSOR_RX_PIN = 10;
constexpr uint8_t SENSOR_TX_PIN = 11;
constexpr uint8_t SENSOR_IO_PIN = 2;

SoftwareSerial sensorSerial(SENSOR_RX_PIN, SENSOR_TX_PIN);
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
  sensorSerial.begin(SENSOR_BAUD);
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
