# Examples

Each example folder contains one Arduino sketch with the same name. The board
folder is part of the example's configuration: UART objects, pins, OT/IO pin,
LED behavior, and networking APIs are intentionally explicit there.

## Board examples

All six board directories contain:

- `DistancePresence`: parses normal-mode UART records and prints
  `distance_cm_raw:X distance_cm_filtered:Y presence:Z`. The built-in LED is
  on while `sensor.isPresent()` is true.
- `GPIOPresence`: reads the active-high OT/IO pin and prints debounced state
  changes. The built-in LED is on while `presencePin.isPresent()` is true.
- `RawAsciiMonitor`: copies raw UART bytes to the monitor without parsing.
- `FilterAndFreshness`: prints filtered/raw values and `data_valid` every 250 ms.
- `GPIOAndUART`: compares UART presence with raw/debounced OT/IO state.

Network-capable board folders also contain `WebDashboard`:

| Board | Network API | Dashboard UART | OT/IO default | Notes |
| --- | --- | --- | --- | --- |
| `RP2040` | `WiFi.h` and `WiFiServer` | `Serial1`, GP1 RX / GP0 TX | GP2 | Pico W station-mode connection. |
| `ESP32` | `WiFi.h` and `WiFiServer` | UART2, GPIO16 RX / GPIO17 TX | GPIO4 | GPIO4 avoids the common GPIO2 LED pin. |
| `ESP32S3` | `WiFi.h` and `WiFiServer` | UART1, GPIO16 RX / GPIO17 TX | GPIO4 | Check the actual board pinout. |
| `UNO_R4` | `WiFiS3.h` and `WiFiServer` | `Serial1`, D0 RX / D1 TX | D2 | UNO R4 WiFi only. |
| `UNO_Q` | `Arduino_RouterBridge` and `BridgeTCPServer` | `Serial1` | GPIO2 | Host computer supplies network access. |

Classic `UNO` has no web dashboard because it has no onboard network interface.
Its UART examples use `SoftwareSerial` on D10/D11 and may not keep up at
115200 baud.

## Dashboard setup

1. Open the board's `WebDashboard/WebDashboard.ino`.
2. Replace `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD` locally for Pico W,
   ESP32, ESP32-S3, or UNO R4 WiFi. Do not commit real credentials.
3. Select the matching board and upload.
4. Open the serial monitor at 115200 baud and browse to the printed URL.
5. For UNO Q, make sure the host-side RouterBridge network service is running,
   then browse to the host's address on port 80.

The dashboard uses a one-second HTML refresh. It reports raw distance, filtered
distance, UART presence, raw OT/IO, debounced OT/IO, and UART freshness. The
UART parser and GPIO helper continue to be serviced in the main loop.

## Arduino IDE workflow

Install the matching board package, install this library, open the desired
`.ino`, select the board, and open the serial monitor at 115200 baud. Cross the
sensor UART directions and connect sensor ground to board ground. Follow the
voltage warning in the main README before wiring a 5 V board.

## Arduino CLI compile matrix

From the parent directory of this checkout:

```bash
LIB=/path/to/LD2402
LIB_PARENT=/path/to/parent-of-LD2402
BUILD_ROOT=$(mktemp -d /tmp/ld2402-build.XXXXXX)

compile_board() {
  label=$1
  fqbn=$2
  board_dir=$3
  shift 3
  for example in "$@"; do
    arduino-cli compile --warnings none \
      --libraries "$LIB_PARENT" \
      --build-path "$BUILD_ROOT/${label}_${example}" \
      --fqbn "$fqbn" "$LIB/examples/$board_dir/$example"
  done
}

compile_board rp2040 rp2040:rp2040:rpipicow RP2040 \
  DistancePresence GPIOPresence RawAsciiMonitor FilterAndFreshness GPIOAndUART WebDashboard
compile_board esp32 esp32:esp32:esp32 ESP32 \
  DistancePresence GPIOPresence RawAsciiMonitor FilterAndFreshness GPIOAndUART WebDashboard
compile_board esp32s3 esp32:esp32:esp32s3 ESP32S3 \
  DistancePresence GPIOPresence RawAsciiMonitor FilterAndFreshness GPIOAndUART WebDashboard
compile_board uno arduino:avr:uno UNO \
  DistancePresence GPIOPresence RawAsciiMonitor FilterAndFreshness GPIOAndUART
compile_board unor4 arduino:renesas_uno:unor4wifi UNO_R4 \
  DistancePresence GPIOPresence RawAsciiMonitor FilterAndFreshness GPIOAndUART WebDashboard
compile_board unoq arduino:zephyr:unoq UNO_Q \
  DistancePresence GPIOPresence RawAsciiMonitor FilterAndFreshness GPIOAndUART WebDashboard
```

The UNO Q build requires the `Arduino_RouterBridge` dependency and its related
libraries installed in the Arduino sketchbook. These builds prove compilation
only; upload, Wi-Fi credentials, host networking, wiring, and physical sensor
behavior require the actual hardware.

## Output examples

Distance example, suitable for Arduino IDE Serial Plotter:

```text
distance_cm_raw:158 distance_cm_filtered:158 presence:1
```

GPIO example:

```text
io_raw:1 presence:1
```

Filter example adds freshness:

```text
distance_cm_raw:158 distance_cm_filtered:157 presence:1 data_valid:1
```
