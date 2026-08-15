# Preserved extras

This directory holds material from the original working checkout that is not
part of the Arduino library API.

- `legacy-sketches/LD2402_RawReader/` contains the original raw hexadecimal
  UART reader. It is retained for protocol troubleshooting and comparison.
- `vendor/HLK-LD2402_TOOL(v1.0.1.9_20250318)/` is retained locally for
  development evidence only. It contains third-party DLLs, firmware, the
  supplied V1.09 manual, logs, and saved radar data. It is ignored by Git and
  is excluded from the public release until redistribution rights are known.
- `metadata/` contains local macOS metadata moved out of the project tree.

See `docs/RELEASE_AUDIT.md` for the public-release inclusion and licensing
decision.

The supported library interface is under `src/`; use the sketches under
`examples/` for normal development.
