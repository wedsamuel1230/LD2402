/*
  HLK-LD2402 GPIO-only presence example for an ESP32-S3 board.

  Manual reference: HLK-LD2402 V1.09, section 3, J2 pin table (PDF page 6):
  J2 pin 2 OT/IO is high for a detected person and low otherwise.

  Wiring:
    LD2402 J2 pin 2 OT/IO -> GPIO4. GPIO2 may be connected to the board's
    built-in LED.
    LD2402 J2 pin 3 GND -> board GND.

  GPIO2 is only a default. Check the pinout of the specific ESP32-S3 board.
  The OT/IO signal is 3.3 V and is driven by the sensor.
*/

#include <Arduino.h>
#include <LD2402.h>

constexpr uint8_t SENSOR_IO_PIN = 4;  // ESP32-S3 GPIO4; keep LED on GPIO2 free
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t DEBOUNCE_TIME_MS = 30UL;
constexpr uint8_t STATUS_LED_PIN = 2;  // Change to the actual board LED pin

LD2402PresencePin presencePin(SENSOR_IO_PIN, DEBOUNCE_TIME_MS);

void emitPresenceState() {
  Serial.print(F("io_raw:"));
  Serial.print(presencePin.rawPresence() ? 1 : 0);
  Serial.print(F(" presence:"));
  Serial.println(presencePin.isPresent() ? 1 : 0);
}

void updateStatusLed() {
  digitalWrite(STATUS_LED_PIN, presencePin.isPresent() ? HIGH : LOW);
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  pinMode(STATUS_LED_PIN, OUTPUT);
  presencePin.begin();
  updateStatusLed();
  emitPresenceState();
}

void loop() {
  if (presencePin.update()) {
    updateStatusLed();
    emitPresenceState();
  }
}
