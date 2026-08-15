/*
  HLK-LD2402 web dashboard for Arduino UNO Q.

  UNO Q uses Arduino_RouterBridge to expose a TCP listener through the host
  computer. The host's network connection supplies the IP address; configure
  network credentials on the UNO Q host, not in this sketch. The listener is
  bound to 0.0.0.0:80 so open the host's address in a browser.

  Wiring: T/TX -> UNO Q Serial1 RX, R/RX -> Serial1 TX, OT/IO -> GPIO2,
  GND -> UNO Q GND. Verify the board package pin map and 3.3 V safety.
*/

#include <Arduino.h>
#include <Arduino_RouterBridge.h>
#include <LD2402.h>

constexpr uint32_t SENSOR_BAUD = 115200UL;
constexpr uint32_t MONITOR_BAUD = 115200UL;
constexpr uint32_t DATA_MAX_AGE_MS = 1000UL;
constexpr uint8_t SENSOR_IO_PIN = 2;

LD2402 sensor(Serial1);
LD2402PresencePin presencePin(SENSOR_IO_PIN, 30UL);
IPAddress anyAddress(0, 0, 0, 0);
BridgeTCPServer<> webServer(Bridge, anyAddress, 80);

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

void serveClient(BridgeTCPClient<>& client) {
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

void setup() {
  Serial1.begin(SENSOR_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);
  presencePin.begin();

  Bridge.begin();
  Monitor.begin(MONITOR_BAUD);
  webServer.begin();
  Monitor.println(F("UNO Q dashboard listening on host port 80"));
}

void loop() {
  sensor.update();
  presencePin.update();
  digitalWrite(LED_BUILTIN, sensor.isPresent() ? HIGH : LOW);

  BridgeTCPClient<> client = webServer.accept();
  if (client.connected()) {
    serveClient(client);
    webServer.disconnect();
  }
}
