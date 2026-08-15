/*
  HLK-LD2402 UART reader for an ESP32-S3 board.

  Wiring used by this example:
    LD2402 T/TX -> GPIO16 (UART1 RX)
    LD2402 R/RX -> GPIO17 (UART1 TX)
    LD2402 G    -> board GND

  ESP32-S3 boards expose different free pins. Change SENSOR_RX_PIN and
  SENSOR_TX_PIN together with your wiring when necessary. The LD2402 UART is
  3.3 V logic; do not drive its RX input from a higher-voltage GPIO.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;

#if defined(ARDUINO_ARCH_ESP32)
#include <HardwareSerial.h>
constexpr int SENSOR_RX_PIN = 16;
constexpr int SENSOR_TX_PIN = 17;
HardwareSerial sensorSerial(1);
LD2402 sensor(sensorSerial);
#elif defined(ARDUINO_ARCH_AVR)
#include <SoftwareSerial.h>
SoftwareSerial sensorSerial(10, 11);
LD2402 sensor(sensorSerial);
#elif defined(ARDUINO_ARCH_RP2040)
LD2402 sensor(Serial1);
#else
LD2402 sensor(Serial1);
#endif

void beginSensorSerial() {
#if defined(ARDUINO_ARCH_ESP32)
  sensorSerial.begin(SENSOR_BAUD, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
#elif defined(ARDUINO_ARCH_AVR)
  sensorSerial.begin(SENSOR_BAUD);
#elif defined(ARDUINO_ARCH_RP2040)
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(SENSOR_BAUD);
#else
  Serial1.begin(SENSOR_BAUD);
#endif
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  beginSensorSerial();
}

void loop() {
  if (sensor.update()) {
    Serial.print(F("distance_cm_raw:"));
    Serial.print(sensor.getDistanceRawCm());
    Serial.print(F(" distance_cm_filtered:"));
    Serial.print(sensor.getDistanceFilteredCm());
    Serial.print(F(" presence:"));
    Serial.println(sensor.isPresent() ? 1 : 0);
  }
}
