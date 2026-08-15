/*
  Cross-board LD2402 distance and presence example.

  This sketch demonstrates the complete public API: raw distance, the
  optional eight-sample filter, presence, and data freshness. It prints at a
  steady rate so it is useful in a Serial Monitor or a simple logger.

  UART wiring:
    LD2402 T/TX -> the selected board UART RX
    LD2402 R/RX -> the selected board UART TX
    LD2402 G    -> board GND

  The sensor uses 115200 8N1 and 3.3 V UART logic. On a classic 5 V UNO, use
  level shifting on the UNO TX -> LD2402 RX path and verify the return level.
  SoftwareSerial is included only as a fallback; 115200 baud may be unreliable
  on an AVR UNO. Prefer a second hardware UART or an external UART adapter.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t PRINT_INTERVAL_MS = 250UL;
constexpr uint32_t DATA_MAX_AGE_MS = 1000UL;
constexpr bool ENABLE_DISTANCE_FILTER = true;

#if defined(ARDUINO_ARCH_AVR)
#include <SoftwareSerial.h>
constexpr uint8_t SENSOR_RX_PIN = 10;
constexpr uint8_t SENSOR_TX_PIN = 11;
SoftwareSerial sensorSerial(SENSOR_RX_PIN, SENSOR_TX_PIN);
LD2402 sensor(sensorSerial);
#elif defined(ARDUINO_ARCH_ESP32)
#include <HardwareSerial.h>
constexpr int SENSOR_RX_PIN = 16;
constexpr int SENSOR_TX_PIN = 17;
HardwareSerial sensorSerial(2);
LD2402 sensor(sensorSerial);
#elif defined(ARDUINO_ARCH_RP2040)
constexpr uint8_t SENSOR_RX_PIN = 1;
constexpr uint8_t SENSOR_TX_PIN = 0;
LD2402 sensor(Serial1);
#else
// UNO R4 and UNO Q use the board package's Serial1 pin mapping.
LD2402 sensor(Serial1);
#endif

void beginSensorSerial() {
#if defined(ARDUINO_ARCH_AVR)
  sensorSerial.begin(SENSOR_BAUD);
#elif defined(ARDUINO_ARCH_ESP32)
  sensorSerial.begin(SENSOR_BAUD, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
#elif defined(ARDUINO_ARCH_RP2040)
  // setTX/setRX are isolated here because they are Arduino-Pico APIs.
  Serial1.setTX(SENSOR_TX_PIN);
  Serial1.setRX(SENSOR_RX_PIN);
  Serial1.begin(SENSOR_BAUD);
#else
  Serial1.begin(SENSOR_BAUD);
#endif
}

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
  beginSensorSerial();
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
