# Examples

Every example directory contains one Arduino sketch. In Arduino IDE, open the
`.ino` file inside the example directory. In Arduino CLI, compile the directory
that contains the `.ino` file.

## Board-specific UART readers

These sketches print the same labelled numeric fields as the original working
plot reader:

```text
distance_cm_raw:158 distance_cm_filtered:158 presence:1
```

| Board directory | Sketch | Serial setup | Default sensor pins |
| --- | --- | --- | --- |
| `RP2040/UART_Reader` | `UART_Reader.ino` | Pico W `Serial1` | GP0 TX, GP1 RX |
| `ESP32/UART_Reader` | `UART_Reader.ino` | ESP32 `HardwareSerial(2)` | GPIO17 TX, GPIO16 RX |
| `ESP32S3/UART_Reader` | `UART_Reader.ino` | ESP32-S3 `HardwareSerial(1)` | GPIO17 TX, GPIO16 RX |

The ESP32-S3 pin choice is an example only. Board variants often reserve
different GPIO, so change the constants in that sketch to match the board.

The classic UNO, UNO R4, and UNO Q are covered by the shared distance example.
The UNO fallback uses `SoftwareSerial`; at 115200 baud it may not be reliable.

## Shared examples

### `Common/DistancePresence`

This cross-board sketch shows:

- `getDistanceRawCm()`;
- `getDistanceFilteredCm()`;
- `isPresent()`;
- `isDataValid()`;
- enabling or disabling the filter with `ENABLE_DISTANCE_FILTER`.

It calls `sensor.update()` on every loop and prints a sample every 250 ms. The
`data_valid` field becomes `0` after the configured freshness interval without
a valid record, while the last parsed presence state remains available.

### `Common/GPIOPresence`

This is the UART-free use case. Connect LD2402 J2 pin 2 OT/IO to the configured
input, which defaults to pin `2` (GP2 on a Pico W), and J2 pin 3 to board GND.
The sketch prints an initial state and then only prints debounced state changes.

## Arduino IDE workflow

1. Install the board package for the selected board.
2. Install or place this library in the Arduino libraries directory.
3. Open one `.ino` file from the tree above.
4. Select the matching board and serial monitor baud rate of 115200.
5. Wire sensor TX to board RX and sensor RX to board TX.

The sensor must be powered according to the supplied manual. Do not infer
power or logic levels from a successful compile.

## Compile checks

From the parent directory of this checkout, set `LIB` and `LIB_PARENT` to the
absolute library path and its parent directory. This script compiles every
example on all five target boards and gives every build an isolated directory:

```bash
LIB=/path/to/LD2402
LIB_PARENT=/path/to/parent-of-LD2402
BUILD_ROOT=$(mktemp -d /tmp/ld2402-build.XXXXXX)

examples=(
  examples/RP2040/UART_Reader
  examples/ESP32/UART_Reader
  examples/ESP32S3/UART_Reader
  examples/Common/DistancePresence
  examples/Common/GPIOPresence
)
labels=(rp2040 esp32 uno r4 unoq)
fqbns=(
  rp2040:rp2040:rpipicow
  esp32:esp32:esp32
  arduino:avr:uno
  arduino:renesas_uno:unor4wifi
  arduino:zephyr:unoq
)

for example in "${examples[@]}"; do
  example_id=${example//\//_}
  for index in "${!fqbns[@]}"; do
    arduino-cli compile --warnings none \
      --libraries "$LIB_PARENT" \
      --build-path "$BUILD_ROOT/${example_id}_${labels[$index]}" \
      --fqbn "${fqbns[$index]}" "$LIB/$example"
  done
done
```

The UNO Q core also needs the `Arduino_RouterBridge` library installed. These
commands prove compilation only; upload, wiring, and runtime behavior require
the physical board and sensor.
