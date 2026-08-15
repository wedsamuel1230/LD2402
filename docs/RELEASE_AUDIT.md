# Release audit

This release contains the Arduino library source, documentation, examples, and
the user-authored raw UART troubleshooting sketch. It does not contain the
local vendor tool bundle.

## Included

- `src/` library source;
- `examples/` board and shared sketches;
- `docs/` protocol and example documentation;
- `extras/legacy-sketches/LD2402_RawReader/` for raw UART diagnostics;
- `README.md`, `LICENSE`, `library.properties`, and `keywords.txt`.

## Excluded

`extras/vendor/` is a local development archive containing third-party Windows
DLLs, a configuration executable, firmware, the supplied manual, logs, and
saved radar data. Its redistribution terms are not established in this
repository, and the logs/data may contain machine-specific or session-specific
information. The directory remains on the developer's machine but is ignored
by Git and is not part of the public release.

macOS `.DS_Store` files and build artifacts are also excluded by `.gitignore`.

The protocol documentation was written from the supplied V1.09 manual during
development; excluding the vendor archive does not change the parser or the
documented normal-mode protocol.

## Verification boundary

The installed Arduino CLI matrix compiled all five examples on all five target
FQBNs (25/25). This is build evidence only. The release does not claim fresh
upload or runtime proof for every board.
