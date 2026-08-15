/*
  HLK-LD2402 web dashboard for an ESP32 Dev Module.

  Put the real network values in this sketch locally. Never commit them.
  The dashboard is served at port 80 and refreshes once per second.

  Wiring: T/TX -> GPIO16, R/RX -> GPIO17, OT/IO -> GPIO4, GND -> ESP32 GND.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include <LD2402.h>

const char WIFI_SSID[] = "YOUR_WIFI_SSID";
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t DATA_MAX_AGE_MS = 1000UL;
constexpr int SENSOR_RX_PIN = 16;
constexpr int SENSOR_TX_PIN = 17;
constexpr uint8_t SENSOR_IO_PIN = 4;
constexpr uint8_t STATUS_LED_PIN = 2;

HardwareSerial sensorSerial(2);
LD2402 sensor(sensorSerial);
LD2402PresencePin presencePin(SENSOR_IO_PIN, 30UL);
WiFiServer webServer(80);

void writeDashboard(Print& output) {
  output.println(F("HTTP/1.1 200 OK"));
  output.println(F("Content-Type: text/html; charset=utf-8"));
  output.println(F("Connection: close"));
  output.println(F("Cache-Control: no-store"));
  output.println();
  output.println(F("<!doctype html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><meta http-equiv=\"refresh\" content=\"1\"><title>LD2402</title><style>body{font-family:system-ui;margin:2rem;max-width:42rem}table{border-collapse:collapse;width:100%}td,th{border-bottom:1px solid #ccc;padding:.65rem;text-align:left}h1{font-size:1.5rem}</style></head><body>"));
  output.println(F("<h1>HLK-LD2402 status</h1><table>"));
  output.print(F("<tr><th>Distance raw</th><td>"));
  output.print(sensor.getDistanceRawCm());
  output.println(F(" cm</td></tr>"));
  output.print(F("<tr><th>Distance filtered</th><td>"));
  output.print(sensor.getDistanceFilteredCm());
  output.println(F(" cm</td></tr>"));
  output.print(F("<tr><th>UART presence</th><td>"));
  output.print(sensor.isPresent() ? F("present") : F("clear"));
  output.println(F("</td></tr>"));
  output.print(F("<tr><th>OT/IO raw</th><td>"));
  output.print(presencePin.rawPresence() ? F("high") : F("low"));
  output.println(F("</td></tr>"));
  output.print(F("<tr><th>OT/IO debounced</th><td>"));
  output.print(presencePin.isPresent() ? F("present") : F("clear"));
  output.println(F("</td></tr>"));
  output.print(F("<tr><th>UART freshness</th><td>"));
  if (sensor.isDataValid(DATA_MAX_AGE_MS)) {
    output.print(F("valid, "));
    output.print(millis() - sensor.lastUpdateMs());
    output.print(F(" ms old"));
  } else {
    output.print(F("stale or no data"));
  }
  output.println(F("</td></tr></table></body></html>"));
}

void serveClient(WiFiClient& client) {
  bool currentLineIsBlank = true;
  bool requestDone = false;
  const uint32_t startedAt = millis();

  while (client.connected() && !requestDone && millis() - startedAt < 1000UL) {
    while (client.available() > 0) {
      const char character = static_cast<char>(client.read());
      if (character == '\n' && currentLineIsBlank) {
        requestDone = true;
        break;
      }
      if (character == '\n') {
        currentLineIsBlank = true;
      } else if (character != '\r') {
        currentLineIsBlank = false;
      }
    }
    yield();
  }

  if (requestDone) {
    writeDashboard(client);
  }
  client.stop();
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();
  Serial.print(F("Dashboard: http://"));
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(MONITOR_BAUD);
  sensorSerial.begin(SENSOR_BAUD, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
  pinMode(STATUS_LED_PIN, OUTPUT);
  presencePin.begin();
  connectWiFi();
  webServer.begin();
}

void loop() {
  sensor.update();
  presencePin.update();
  digitalWrite(STATUS_LED_PIN, sensor.isPresent() ? HIGH : LOW);

  WiFiClient client = webServer.accept();
  if (client) {
    serveClient(client);
  }
}
