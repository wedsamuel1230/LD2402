/*
  HLK-LD2402 text-stream reader for Arduino UNO Q.

  Wiring:
    LD2402 GND -> UNO Q GND.
    LD2402 T/TX -> the UNO Q board package's Serial1 RX pin.
    LD2402 R/RX -> the UNO Q board package's Serial1 TX pin.
    LD2402 OT/IO -> the selected 3.3 V-safe GPIO when GPIO presence is needed.

  The UNO Q pin mapping is board-package dependent; verify the selected
  Serial1 pins against the UNO Q documentation. The LD2402 UART and OT signal
  are 3.3 V logic. Do not drive a sensor input from a higher-voltage output.
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
