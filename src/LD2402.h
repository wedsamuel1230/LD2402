#ifndef LD2402_H
#define LD2402_H

#include <Arduino.h>
#include <stdint.h>

// Parser for the HLK-LD2402 normal-mode ASCII output.
// The sensor sends records such as "distance:158\r\n" and "OFF\r\n".
class LD2402 {
public:
  explicit LD2402(Stream& stream);

  // Consume all currently available bytes without waiting for more input.
  // Returns true when at least one complete, valid sensor record was parsed.
  bool update();

  // Alias for sketches that prefer a read-style name.
  bool read();

  // Filtering is enabled by default and matches the original eight-sample
  // moving average used by the PlotReader sketch.
  void setFilterEnabled(bool enabled);
  bool isFilterEnabled() const;

  uint16_t getDistanceRawCm() const;
  uint16_t getDistanceFilteredCm() const;
  bool isPresent() const;

  // A record is valid only until maxAgeMs has elapsed since the last valid
  // record. Before the first valid record this always returns false.
  bool isDataValid(uint32_t maxAgeMs = 1000UL) const;
  uint32_t lastUpdateMs() const;

private:
  enum {
    kLineBufferSize = 32,
    kFilterWindowSize = 8
  };

  bool processCompleteLine();
  bool parseLine(bool& present, uint16_t& distanceCm) const;
  void resetFilter();
  uint16_t updateFilter(uint16_t distanceCm);

  static bool isHorizontalSpace(char value);
  static const char* skipHorizontalSpace(const char* text);

  Stream& _stream;
  char _lineBuffer[kLineBufferSize];
  uint8_t _lineLength;
  bool _lineOverflow;

  uint16_t _filterSamples[kFilterWindowSize];
  uint8_t _filterCount;
  uint8_t _filterIndex;
  uint32_t _filterSum;
  bool _filterEnabled;

  uint16_t _distanceRawCm;
  uint16_t _distanceFilteredCm;
  bool _present;
  bool _dataValid;
  uint32_t _lastUpdateMs;
};

// Lightweight helper for the LD2402 OT/IO presence output. The sensor drives
// this pin high when a person is detected and low otherwise.
class LD2402PresencePin {
public:
  explicit LD2402PresencePin(uint8_t pin, uint32_t debounceMs = 30UL);

  void begin();

  // Returns true only when the debounced state changes.
  bool update();

  bool isPresent() const;
  bool rawPresence() const;
  uint8_t pin() const;

private:
  uint8_t _pin;
  uint32_t _debounceMs;
  bool _rawPresence;
  bool _stablePresence;
  bool _initialized;
  uint32_t _rawChangedAt;
};

#endif
