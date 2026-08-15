/*
  HLK-LD2402 text-stream reader for an ESP32-S3 board.

  Normal-mode records are documented in the HLK-LD2402 V1.09 manual,
  section 5.6.1 (PDF page 24): distance:158\r\n and OFF\r\n.

  Wiring used by this example:
    LD2402 GND -> board GND.
    LD2402 T/TX -> GPIO16 (UART1 RX).
    LD2402 R/RX -> GPIO17 (UART1 TX).
    LD2402 OT/IO -> GPIO4 when GPIO presence is also needed. GPIO2 may be
                    connected to the board's built-in LED.

  ESP32-S3 boards expose different free pins. Change the constants together
  with your wiring when necessary. The LD2402 UART and OT signal are 3.3 V
  logic; do not drive a sensor input from a higher-voltage GPIO.
*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr bool ENABLE_DISTANCE_FILTER = true;

constexpr int SENSOR_RX_PIN = 16;  // GPIO16 <- LD2402 T/TX
constexpr int SENSOR_TX_PIN = 17;  // GPIO17 -> LD2402 R/RX
constexpr uint8_t STATUS_LED_PIN = 2;  // Default external/status LED pin

HardwareSerial sensorSerial(1);
LD2402 sensor(sensorSerial);

void beginSensorSerial() {
  sensorSerial.begin(SENSOR_BAUD, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
}

void emitPlotSample() {
  Serial.print(F("distance_cm_raw:"));
  Serial.print(sensor.getDistanceRawCm());
  Serial.print(F(" distance_cm_filtered:"));
  Serial.print(sensor.getDistanceFilteredCm());
  Serial.print(F(" presence:"));
  Serial.println(sensor.isPresent() ? 1 : 0);
}

void updateStatusLed() {
  digitalWrite(STATUS_LED_PIN, sensor.isPresent() ? HIGH : LOW);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  beginSensorSerial();
  sensor.setFilterEnabled(ENABLE_DISTANCE_FILTER);
  pinMode(STATUS_LED_PIN, OUTPUT);
  updateStatusLed();
}

void loop() {
  if (sensor.update()) {
    updateStatusLed();
    emitPlotSample();
  }
}
