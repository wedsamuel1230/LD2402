/*
  HLK-LD2402 text-stream reader for Arduino UNO R4.

  Wiring:
    LD2402 GND -> UNO R4 GND.
    LD2402 T/TX -> D0 (Serial1 RX).
    LD2402 R/RX -> D1 (Serial1 TX).
    LD2402 OT/IO -> D2 when GPIO presence is also needed.

  The LD2402 UART and OT signal are 3.3 V logic. Do not drive a sensor input
  directly from a higher-voltage board output without level shifting.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr bool ENABLE_DISTANCE_FILTER = true;

LD2402 sensor(Serial1);

void updateStatusLed() {
  digitalWrite(LED_BUILTIN, sensor.isPresent() ? HIGH : LOW);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  Serial1.begin(SENSOR_BAUD);
  sensor.setFilterEnabled(ENABLE_DISTANCE_FILTER);
  pinMode(LED_BUILTIN, OUTPUT);
  updateStatusLed();
}

void loop() {
  if (sensor.update()) {
    updateStatusLed();
    Serial.print(F("distance_cm_raw:"));
    Serial.print(sensor.getDistanceRawCm());
    Serial.print(F(" distance_cm_filtered:"));
    Serial.print(sensor.getDistanceFilteredCm());
    Serial.print(F(" presence:"));
    Serial.println(sensor.isPresent() ? 1 : 0);
  }
}
