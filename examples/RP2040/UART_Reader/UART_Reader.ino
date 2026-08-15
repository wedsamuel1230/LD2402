/*
  HLK-LD2402 text-stream reader for Arduino IDE Serial Plotter.

  The library parses the normal-mode records documented in the HLK-LD2402
  V1.09 manual, section 5.6.1 (PDF page 24): distance:158\r\n and OFF\r\n.
  Engineering-mode binary frames are intentionally not interpreted here.

  Wiring:
    LD2402 GND -> board GND on every board.
    Pico W: LD2402 T/TX -> GP1 (Serial1 RX), R/RX -> GP0 (Serial1 TX).
             The Arduino-Pico setTX/setRX calls below make this explicit.
    ESP32:   LD2402 T/TX -> GPIO16 (UART2 RX), R/RX -> GPIO17 (UART2 TX).
    UNO R4:  LD2402 T/TX -> D0/Serial1 RX, R/RX -> D1/Serial1 TX.
    UNO Q:   use the board package's Serial1 RX/TX pins; verify its pinout.
    UNO AVR: SoftwareSerial uses D10 as RX and D11 as TX. At 115200 baud it
             may be unreliable; a hardware UART or external UART adapter is
             recommended. Use level shifting for any 5 V output to the sensor.

  The LD2402 UART and OT signal are 3.3 V logic. Do not drive a sensor input
  directly from a 5 V board output without an appropriate level shifter.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr bool ENABLE_DISTANCE_FILTER = true;

#if defined(ARDUINO_ARCH_AVR)
#include <SoftwareSerial.h>
constexpr uint8_t SENSOR_RX_PIN = 10;  // Board RX <- LD2402 T/TX
constexpr uint8_t SENSOR_TX_PIN = 11;  // Board TX -> LD2402 R/RX
SoftwareSerial sensorSerial(SENSOR_RX_PIN, SENSOR_TX_PIN);
LD2402 sensor(sensorSerial);
#elif defined(ARDUINO_ARCH_ESP32)
#include <HardwareSerial.h>
constexpr int SENSOR_RX_PIN = 16;  // UART2 RX <- LD2402 T/TX
constexpr int SENSOR_TX_PIN = 17;  // UART2 TX -> LD2402 R/RX
HardwareSerial sensorSerial(2);
LD2402 sensor(sensorSerial);
#elif defined(ARDUINO_ARCH_RP2040)
constexpr int SENSOR_RX_PIN = 1;  // Pico W GP1 <- LD2402 T/TX
constexpr int SENSOR_TX_PIN = 0;  // Pico W GP0 -> LD2402 R/RX
LD2402 sensor(Serial1);
#else
// UNO R4 and UNO Q expose a board-defined Serial1. Check their pinout.
LD2402 sensor(Serial1);
#endif

void beginSensorSerial() {
#if defined(ARDUINO_ARCH_AVR)
  // SoftwareSerial is a compile-time fallback for the classic UNO.
  sensorSerial.begin(SENSOR_BAUD);
#elif defined(ARDUINO_ARCH_ESP32)
  sensorSerial.begin(SENSOR_BAUD, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
#elif defined(ARDUINO_ARCH_RP2040)
  // setTX/setRX are Arduino-Pico APIs; they are isolated to this example.
  Serial1.setTX(SENSOR_TX_PIN);
  Serial1.setRX(SENSOR_RX_PIN);
  Serial1.begin(SENSOR_BAUD);
#else
  Serial1.begin(SENSOR_BAUD);
#endif
}

void emitPlotSample() {
  // Numeric labelled fields are understood by Arduino IDE Serial Plotter.
  Serial.print(F("distance_cm_raw:"));
  Serial.print(sensor.getDistanceRawCm());
  Serial.print(F(" distance_cm_filtered:"));
  Serial.print(sensor.getDistanceFilteredCm());
  Serial.print(F(" presence:"));
  Serial.println(sensor.isPresent() ? 1 : 0);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  beginSensorSerial();
  sensor.setFilterEnabled(ENABLE_DISTANCE_FILTER);
}

void loop() {
  if (sensor.update()) {
    emitPlotSample();
  }
}
