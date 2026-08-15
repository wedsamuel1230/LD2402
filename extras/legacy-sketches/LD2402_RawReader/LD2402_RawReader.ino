/*
  HLK-LD2402 raw UART reader

  Arduino IDE targets:
    - Raspberry Pi Pico W with the Arduino-Pico core
    - Arduino UNO Q with its Arduino board package

  This sketch does not decode distance, presence, or energy gates. It only
  copies bytes received from the LD2402 to the USB serial monitor as hex.
*/

constexpr uint32_t SENSOR_BAUD = 115200;
constexpr uint32_t MONITOR_BAUD = 115200;

// The Arduino-Pico core defaults Serial1 to GP0 (TX) and GP1 (RX). Keep these
// explicit so the wiring is clear; the UNO Q branch uses its board variant.
#if defined(ARDUINO_ARCH_RP2040)
constexpr int SENSOR_TX_PIN = 0;  // Pico W GP0 -> LD2402 R
constexpr int SENSOR_RX_PIN = 1;  // Pico W GP1 <- LD2402 T
#endif

uint32_t bytesPrinted = 0;

void setup() {
  Serial.begin(MONITOR_BAUD);

  // Do not block forever if the board is powered without a USB host attached.
  const uint32_t waitStart = millis();
  while (!Serial && (millis() - waitStart < 3000UL)) {
    delay(1);
  }

#if defined(ARDUINO_ARCH_RP2040)
  Serial1.setTX(SENSOR_TX_PIN);
  Serial1.setRX(SENSOR_RX_PIN);
#endif
  Serial1.begin(SENSOR_BAUD);

  Serial.println();
  Serial.println(F("LD2402 raw reader ready"));
  Serial.println(F("UART: 115200 8N1; output is received bytes in hex"));
  Serial.println(F("Connect LD2402 T/TX -> board RX, R/RX -> board TX, and G -> GND"));
}

void loop() {
  while (Serial1.available() > 0) {
    const uint8_t value = static_cast<uint8_t>(Serial1.read());

    if ((bytesPrinted % 16U) == 0U) {
      if (bytesPrinted != 0U) {
        Serial.println();
      }
      Serial.print(F("RX "));
    }

    if (value < 0x10U) {
      Serial.print('0');
    }
    Serial.print(value, HEX);
    Serial.print(' ');
    ++bytesPrinted;
  }
}
