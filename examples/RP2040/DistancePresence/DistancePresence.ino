/*
  HLK-LD2402 text-stream reader for Arduino IDE Serial Plotter.

  The library parses the normal-mode records documented in the HLK-LD2402
  V1.09 manual, section 5.6.1 (PDF page 24): distance:158\r\n and OFF\r\n.
  Engineering-mode binary frames are intentionally not interpreted here.

  Wiring:
    LD2402 GND -> Pico W GND.
    LD2402 T/TX -> Pico W GP1 (Serial1 RX).
    LD2402 R/RX -> Pico W GP0 (Serial1 TX).
    LD2402 OT/IO -> Pico W GP2 when GPIO presence is also needed.

  The LD2402 UART and OT signal are 3.3 V logic. Do not drive a sensor input
  directly from a 5 V board output without an appropriate level shifter.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr bool ENABLE_DISTANCE_FILTER = true;

constexpr uint8_t SENSOR_RX_PIN = 1;  // GP1 <- LD2402 T/TX
constexpr uint8_t SENSOR_TX_PIN = 0;  // GP0 -> LD2402 R/RX

LD2402 sensor(Serial1);

void beginSensorSerial() {
  // setTX/setRX are Arduino-Pico APIs; they are explicit in this board example.
  Serial1.setTX(SENSOR_TX_PIN);
  Serial1.setRX(SENSOR_RX_PIN);
  Serial1.begin(SENSOR_BAUD);
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
  digitalWrite(LED_BUILTIN, sensor.isPresent() ? HIGH : LOW);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  beginSensorSerial();
  sensor.setFilterEnabled(ENABLE_DISTANCE_FILTER);
  pinMode(LED_BUILTIN, OUTPUT);
  updateStatusLed();
}

void loop() {
  if (sensor.update()) {
    updateStatusLed();
    emitPlotSample();
  }
}
