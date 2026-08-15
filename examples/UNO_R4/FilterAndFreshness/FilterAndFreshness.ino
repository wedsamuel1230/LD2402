/*
  LD2402 filter and freshness example for Arduino UNO R4.

  Wiring: sensor T/TX -> D0/Serial1 RX, R/RX -> D1/Serial1 TX, GND -> UNO R4 GND.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t PRINT_INTERVAL_MS = 250UL;
constexpr uint32_t DATA_MAX_AGE_MS = 1000UL;
constexpr bool ENABLE_DISTANCE_FILTER = true;

LD2402 sensor(Serial1);

void printSample() {
  Serial.print(F("distance_cm_raw:"));
  Serial.print(sensor.getDistanceRawCm());
  Serial.print(F(" distance_cm_filtered:"));
  Serial.print(sensor.getDistanceFilteredCm());
  Serial.print(F(" presence:"));
  Serial.print(sensor.isPresent() ? 1 : 0);
  Serial.print(F(" data_valid:"));
  Serial.println(sensor.isDataValid(DATA_MAX_AGE_MS) ? 1 : 0);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  Serial1.begin(SENSOR_BAUD);
  sensor.setFilterEnabled(ENABLE_DISTANCE_FILTER);
}

void loop() {
  sensor.update();

  static uint32_t lastPrintMs = 0;
  const uint32_t now = millis();
  if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
    lastPrintMs = now;
    printSample();
  }
}
