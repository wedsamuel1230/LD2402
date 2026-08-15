# LD2402 Arduino library

Non-blocking Arduino support for the HLK-LD2402 mmWave presence and distance
sensor. The library parses the sensor's normal-mode ASCII UART stream from any
Arduino `Stream` and provides an active-high OT/IO presence-pin helper.

![HLK-LD2402 sensor](docs/ld2402.png)

## Features

- Parses `distance:<cm>` and `OFF` records byte by byte without waiting for
  more input.
- Accepts `Stream&`, so the application chooses `Serial1`, `HardwareSerial`,
  `SoftwareSerial`, or another compatible stream.
- Optional eight-sample moving-average distance filter, enabled by default.
- Freshness reporting with `isDataValid()` and `lastUpdateMs()`.
- Lightweight debounced `LD2402PresencePin` helper for the OT/IO output.
- Fixed-size library buffers, no `String`, no dynamic allocation, and no
  delays in the parser.

## Supported boards

The source uses portable Arduino APIs and the board-specific examples below
show the required UART setup for each target.

| Board | FQBN used for build checks | Sensor UART | OT/IO | Web dashboard |
| --- | --- | --- | --- | --- |
| Raspberry Pi Pico W | `rp2040:rp2040:rpipicow` | `Serial1`, GP1 RX / GP0 TX | GP2 | Yes |
| ESP32 Dev Module | `esp32:esp32:esp32` | UART2, GPIO16 RX / GPIO17 TX | GPIO4 | Yes |
| ESP32-S3 generic | `esp32:esp32:esp32s3` | UART1, GPIO16 RX / GPIO17 TX | GPIO4 | Yes |
| Arduino UNO | `arduino:avr:uno` | `SoftwareSerial`, D10 RX / D11 TX | D2 | No |
| Arduino UNO R4 WiFi | `arduino:renesas_uno:unor4wifi` | `Serial1`, D0 RX / D1 TX | D2 | Yes |
| Arduino UNO Q | `arduino:zephyr:unoq` | board `Serial1` pins | GPIO2 default | Yes, through host |

The ESP32 GPIO4 choice keeps GPIO2 available for the common Dev Module
built-in LED. ESP32-S3 pin assignments are defaults; check the actual board
pinout. UNO Q UART and GPIO mappings are board-package dependent.

The classic UNO example uses `SoftwareSerial` at the sensor's fixed 115200
baud. This may be unreliable; use a hardware UART or external UART adapter
when possible.

## Wiring and voltage

Cross the UART directions: sensor `T/TX` goes to board RX, and sensor `R/RX`
goes to board TX. Connect sensor ground to board ground.

The LD2402 UART and OT/IO signals are 3.3 V. Do not drive sensor RX directly
from a 5 V board output. Use a suitable level shifter or divider and verify
the sensor TX level is safe for the controller input. The supplied HLK-LD2402
V1.09 manual identifies J2 pin 2 as OT/IO and J2 pin 3 as ground.

## Install

Install this repository as the `LD2402` folder in the Arduino libraries
directory, install it from a ZIP, or pass its parent directory with Arduino
CLI's `--libraries` option. Restart the IDE after installing a new library.

## Quick start

Pass an existing serial stream to the parser. The library consumes only bytes
already available, so call `update()` on every loop.

```cpp
#include <Arduino.h>
#include <LD2402.h>

LD2402 sensor(Serial1);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop() {
  if (sensor.update() && sensor.isDataValid()) {
    Serial.print("raw=");
    Serial.print(sensor.getDistanceRawCm());
    Serial.print(" filtered=");
    Serial.print(sensor.getDistanceFilteredCm());
    Serial.print(" present=");
    Serial.println(sensor.isPresent() ? 1 : 0);
  }
}
```

For a Pico W, configure the UART pins before `begin()`:

```cpp
Serial1.setTX(0);
Serial1.setRX(1);
Serial1.begin(115200);
```

For OT/IO-only use:

```cpp
#include <LD2402.h>

LD2402PresencePin presence(2, 30);

void setup() {
  Serial.begin(115200);
  presence.begin();
}

void loop() {
  if (presence.update()) {
    Serial.println(presence.isPresent() ? "present" : "clear");
  }
}
```

## API

### `LD2402`

| API | Description |
| --- | --- |
| `LD2402(Stream& stream)` | Wrap an existing serial stream. |
| `bool update()` | Consume available bytes; true if a valid record was accepted. |
| `bool read()` | Alias for `update()`. |
| `void setFilterEnabled(bool)` | Enable or disable the eight-sample filter. |
| `bool isFilterEnabled()` | Return the filter setting. |
| `uint16_t getDistanceRawCm()` | Most recent raw distance in centimetres. |
| `uint16_t getDistanceFilteredCm()` | Filtered distance, or raw distance when disabled. |
| `bool isPresent()` | Last parsed UART presence state. |
| `bool isDataValid(uint32_t maxAgeMs = 1000)` | True after a valid record until it becomes stale. |
| `uint32_t lastUpdateMs()` | `millis()` time of the last valid record. |

### `LD2402PresencePin`

| API | Description |
| --- | --- |
| `LD2402PresencePin(uint8_t pin, uint32_t debounceMs = 30)` | Configure an active-high OT/IO input. |
| `begin()` | Configure the input and capture its initial state. |
| `update()` | Sample the input; true after a debounced state change. |
| `isPresent()` | Current debounced state. |
| `rawPresence()` | Most recent digital sample. |
| `pin()` | Configured input pin number. |

## Examples

Every board folder contains sketches with that board's complete UART setup;
they do not contain architecture-selection branches.

| Example | Purpose |
| --- | --- |
| `DistancePresence` | Serial Plotter output: `distance_cm_raw:X distance_cm_filtered:Y presence:Z`; LED follows UART presence. |
| `GPIOPresence` | UART-free OT/IO state changes; LED follows debounced GPIO presence. |
| `RawAsciiMonitor` | Copies the raw sensor stream for protocol and wiring diagnostics. |
| `FilterAndFreshness` | Demonstrates filter control, periodic output, and stale-data detection. |
| `GPIOAndUART` | Compares UART presence and OT/IO presence side by side. |
| `WebDashboard` | Browser dashboard on Wi-Fi boards; not available on classic UNO. |

The tree is arranged as:

```text
examples/
├── RP2040/{DistancePresence,GPIOPresence,RawAsciiMonitor,FilterAndFreshness,GPIOAndUART,WebDashboard}/
├── ESP32/{DistancePresence,GPIOPresence,RawAsciiMonitor,FilterAndFreshness,GPIOAndUART,WebDashboard}/
├── ESP32S3/{DistancePresence,GPIOPresence,RawAsciiMonitor,FilterAndFreshness,GPIOAndUART,WebDashboard}/
├── UNO/{DistancePresence,GPIOPresence,RawAsciiMonitor,FilterAndFreshness,GPIOAndUART}/
├── UNO_R4/{DistancePresence,GPIOPresence,RawAsciiMonitor,FilterAndFreshness,GPIOAndUART,WebDashboard}/
└── UNO_Q/{DistancePresence,GPIOPresence,RawAsciiMonitor,FilterAndFreshness,GPIOAndUART,WebDashboard}/
```

Open the `.ino` whose name matches its folder. The dashboard sketches contain
`YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD` placeholders. Replace them locally,
upload, and open the address printed by the serial monitor. UNO Q uses the
Linux host's network through `Arduino_RouterBridge`; open the host address on
port 80 instead of entering Wi-Fi credentials in the sketch.

See [docs/EXAMPLES.md](docs/EXAMPLES.md) for the complete compile matrix and
[docs/LD2402_PROTOCOL.md](docs/LD2402_PROTOCOL.md) for the parser's protocol
scope and frame reference.

## Protocol scope

The parser accepts normal-mode ASCII records such as `distance:158\r\n` and
`OFF\r\n` at `115200 8N1`. It intentionally does not interpret the separate
engineering-mode binary energy frame. The raw monitor example can be used to
inspect bytes before parser integration.

## Limitations

- The library is listen-only; it does not configure the sensor or send commands.
- Normal-mode ASCII is supported; engineering-mode binary frames are not
  parsed by `LD2402`.
- `SoftwareSerial` at 115200 baud may lose data on a classic UNO.
- A successful compile does not prove power, voltage levels, wiring, upload,
  network setup, or physical sensor behavior.
- Web dashboards are example applications, not part of the library API. They
  use each board core's own networking surface and require local credentials or
  UNO Q host networking.

## Manual and vendor files

The supplied HLK-LD2402 manual was used as development reference material.
Third-party vendor binaries and development archives are kept outside the
public library release surface. See [docs/RELEASE_AUDIT.md](docs/RELEASE_AUDIT.md).

## License

MIT. See [LICENSE](LICENSE).
