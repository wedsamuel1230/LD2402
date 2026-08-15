/*
  Compare LD2402 UART presence with its OT/IO signal on Arduino UNO Q.

  Wiring: T/TX -> Serial1 RX, R/RX -> Serial1 TX, OT/IO -> GPIO 2,
  GND -> UNO Q GND. Verify the board package pin map and 3.3 V safety.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t PRINT_INTERVAL_MS = 250UL;
constexpr uint32_t DATA_MAX_AGE_MS = 1000UL;
constexpr uint8_t SENSOR_IO_PIN = 2;

LD2402 sensor(Serial1);
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
  Serial1.begin(SENSOR_BAUD);
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
