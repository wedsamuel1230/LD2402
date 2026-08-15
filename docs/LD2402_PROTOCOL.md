# LD2402 protocol reference

This page records the protocol facts used by the library and the limits of its
parser. The hardware details were checked against the supplied HLK-LD2402 user
manual V1.09 during development. The local vendor archive is intentionally
excluded from the public release; see [the release audit](RELEASE_AUDIT.md).

## UART settings

The normal serial interface is:

| Setting | Value |
| --- | --- |
| Baud rate | 115200 bit/s |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |

The library expects an already-configured Arduino `Stream`. It does not open a
serial port, change the sensor mode, or send configuration commands.

## Normal-mode ASCII records

In the normal output mode, the sensor sends a line for each state update. The
observed/manual-described records are:

```text
distance:158\r\n
OFF\r\n
```

The line meaning is:

| Record | Meaning |
| --- | --- |
| `distance:<integer>` | A target is present; the integer is distance in centimetres. |
| `OFF` | No target is present. |

The line terminator may be CR, LF, or CRLF. `LD2402::update()` consumes bytes
without waiting and accepts a complete record only after a terminator is seen.
The implementation also tolerates horizontal whitespace around the record and
accepts distances from `0` through `65535` centimetres. Invalid or overlong
records are discarded without changing the last valid state.

For a valid distance record:

1. `getDistanceRawCm()` is updated.
2. `isPresent()` becomes true.
3. The optional eight-sample moving average is updated.
4. `lastUpdateMs()` records the acceptance time.

For `OFF`, the distance getters become zero, presence becomes false, and the
filter history is reset. `isDataValid()` describes whether a valid record has
arrived recently; it does not automatically change `isPresent()` when the
stream goes silent.

## Engineering-mode binary frame

The V1.09 manual also documents an engineering/data frame. Its visible shape
is:

```text
F4 F3 F2 F1 | length (little endian) | state | distance (little endian)
             | 128 energy bytes | F8 F7 F6 F5
```

The four-byte header and footer are byte markers. The length and distance
fields are little-endian. The energy section contains 128 bytes. This is a
different output mode from the normal ASCII records above.

`LD2402` deliberately does not decode this binary frame. The raw troubleshooting
reader remains at
`extras/legacy-sketches/LD2402_RawReader/LD2402_RawReader.ino` and copies
received bytes as hexadecimal. Keeping the binary frame raw avoids claiming
that a vendor engineering field has the same meaning or update timing as the
normal-mode distance record.

## OT/IO presence output

The manual's J2 pin table describes the presence output as follows:

| Sensor connection | Function |
| --- | --- |
| J2 pin 2, OT/IO | High when a person is detected; low when no person is detected. Signal range: 0-3.3 V. |
| J2 pin 3, G | Signal ground. Connect to the controller ground. |

`LD2402PresencePin` reads this active-high signal with `digitalRead()`. It
does not use a pull-up or pull-down because the sensor drives the line. The
helper can apply a small debounce interval and reports a state change from
`update()`.

## Logic-level warning

The LD2402 UART and OT/IO signal are 3.3 V interfaces. A Pico W, ESP32, and
ESP32-S3 are normally used with 3.3 V GPIO. A classic 5 V Arduino UNO must not
drive the sensor RX directly from a 5 V TX pin; use an appropriate level
shifter or divider and verify the sensor TX level is accepted by the UNO input.
Check the board and sensor datasheets before wiring any other 5 V controller.

## Parser scope

The parser is intentionally small for the classic UNO's 2 KB SRAM limit:

- fixed 32-byte line buffer;
- fixed eight-sample filter history;
- no `String` objects;
- no dynamic allocation;
- no blocking reads or delays;
- no platform-specific code in `src/`.

These implementation details are documented so a raw engineering-frame reader
is not mistaken for the normal library parser. See the main
[README](../README.md) for installation, board wiring, and the public API.
