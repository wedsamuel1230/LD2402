/*
  HLK-LD2402 text-stream reader for a classic Arduino UNO.

  Wiring:
    LD2402 GND -> UNO GND.
    LD2402 T/TX -> D10 (SoftwareSerial RX).
    LD2402 R/RX -> D11 (SoftwareSerial TX).
    LD2402 OT/IO -> D2 when GPIO presence is also needed.

  The sensor uses 115200 8N1 and 3.3 V logic. SoftwareSerial at 115200 baud
  may be unreliable on an AVR UNO. A hardware UART or external UART adapter is
  recommended. Level-shift the UNO TX signal before it reaches sensor RX.
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr bool ENABLE_DISTANCE_FILTER = true;

constexpr uint8_t SENSOR_RX_PIN = 10;  // D10 <- LD2402 T/TX
constexpr uint8_t SENSOR_TX_PIN = 11;  // D11 -> LD2402 R/RX through level shifter

SoftwareSerial sensorSerial(SENSOR_RX_PIN, SENSOR_TX_PIN);
LD2402 sensor(sensorSerial);

void updateStatusLed() {
  digitalWrite(LED_BUILTIN, sensor.isPresent() ? HIGH : LOW);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  sensorSerial.begin(SENSOR_BAUD);
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
