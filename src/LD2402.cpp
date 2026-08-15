#include "LD2402.h"

#include <stdlib.h>
#include <string.h>

LD2402::LD2402(Stream& stream)
    : _stream(stream),
      _lineLength(0),
      _lineOverflow(false),
      _filterCount(0),
      _filterIndex(0),
      _filterSum(0),
      _filterEnabled(true),
      _distanceRawCm(0),
      _distanceFilteredCm(0),
      _present(false),
      _dataValid(false),
      _lastUpdateMs(0) {
  resetFilter();
}

bool LD2402::update() {
  bool parsedRecord = false;

  while (_stream.available() > 0) {
    const int value = _stream.read();
    if (value < 0) {
      break;
    }

    const char character = static_cast<char>(value);
    if (character == '\r' || character == '\n') {
      if (processCompleteLine()) {
        parsedRecord = true;
      }
      _lineLength = 0;
      _lineOverflow = false;
      continue;
    }

    if (_lineOverflow) {
      continue;
    }

    if (_lineLength < (sizeof(_lineBuffer) - 1U)) {
      _lineBuffer[_lineLength++] = character;
    } else {
      // Discard an overlong record rather than allowing it to corrupt memory.
      _lineOverflow = true;
    }
  }

  return parsedRecord;
}

bool LD2402::read() {
  return update();
}

void LD2402::setFilterEnabled(bool enabled) {
  if (_filterEnabled == enabled) {
    return;
  }

  _filterEnabled = enabled;
  resetFilter();

  if (!enabled) {
    _distanceFilteredCm = _distanceRawCm;
  }
}

bool LD2402::isFilterEnabled() const {
  return _filterEnabled;
}

uint16_t LD2402::getDistanceRawCm() const {
  return _distanceRawCm;
}

uint16_t LD2402::getDistanceFilteredCm() const {
  return _distanceFilteredCm;
}

bool LD2402::isPresent() const {
  return _present;
}

bool LD2402::isDataValid(uint32_t maxAgeMs) const {
  if (!_dataValid) {
    return false;
  }

  return (millis() - _lastUpdateMs) <= maxAgeMs;
}

uint32_t LD2402::lastUpdateMs() const {
  return _lastUpdateMs;
}

bool LD2402::processCompleteLine() {
  if (_lineOverflow || _lineLength == 0) {
    return false;
  }

  _lineBuffer[_lineLength] = '\0';

  bool present = false;
  uint16_t distanceCm = 0;
  if (!parseLine(present, distanceCm)) {
    return false;
  }

  _distanceRawCm = distanceCm;
  _present = present;

  if (present) {
    if (_filterEnabled) {
      _distanceFilteredCm = updateFilter(distanceCm);
    } else {
      _distanceFilteredCm = distanceCm;
    }
  } else {
    resetFilter();
    _distanceFilteredCm = 0;
  }

  _dataValid = true;
  _lastUpdateMs = millis();
  return true;
}

bool LD2402::parseLine(bool& present, uint16_t& distanceCm) const {
  const char* cursor = skipHorizontalSpace(_lineBuffer);

  if (strcmp(cursor, "OFF") == 0) {
    present = false;
    distanceCm = 0;
    return true;
  }

  static const char distanceLabel[] = "distance";
  if (strncmp(cursor, distanceLabel, sizeof(distanceLabel) - 1U) != 0) {
    return false;
  }

  cursor += sizeof(distanceLabel) - 1U;
  cursor = skipHorizontalSpace(cursor);

  if (*cursor != ':') {
    return false;
  }
  cursor = skipHorizontalSpace(cursor + 1);

  char* end = nullptr;
  const long parsed = strtol(cursor, &end, 10);
  if (end == cursor || parsed < 0 || parsed > 65535L) {
    return false;
  }

  while (isHorizontalSpace(*end)) {
    ++end;
  }
  if (*end != '\0') {
    return false;
  }

  present = true;
  distanceCm = static_cast<uint16_t>(parsed);
  return true;
}

void LD2402::resetFilter() {
  _filterCount = 0;
  _filterIndex = 0;
  _filterSum = 0;
}

uint16_t LD2402::updateFilter(uint16_t distanceCm) {
  if (_filterCount < kFilterWindowSize) {
    _filterSamples[_filterIndex] = distanceCm;
    _filterSum += distanceCm;
    ++_filterCount;
  } else {
    _filterSum -= _filterSamples[_filterIndex];
    _filterSamples[_filterIndex] = distanceCm;
    _filterSum += distanceCm;
  }

  _filterIndex = static_cast<uint8_t>((_filterIndex + 1U) % kFilterWindowSize);
  return static_cast<uint16_t>(_filterSum / _filterCount);
}

bool LD2402::isHorizontalSpace(char value) {
  return value == ' ' || value == '\t';
}

const char* LD2402::skipHorizontalSpace(const char* text) {
  while (isHorizontalSpace(*text)) {
    ++text;
  }
  return text;
}

LD2402PresencePin::LD2402PresencePin(uint8_t pin, uint32_t debounceMs)
    : _pin(pin),
      _debounceMs(debounceMs),
      _rawPresence(false),
      _stablePresence(false),
      _initialized(false),
      _rawChangedAt(0) {}

void LD2402PresencePin::begin() {
  pinMode(_pin, INPUT);

  const uint32_t now = millis();
  _rawPresence = digitalRead(_pin) == HIGH;
  _stablePresence = _rawPresence;
  _rawChangedAt = now;
  _initialized = true;
}

bool LD2402PresencePin::update() {
  if (!_initialized) {
    return false;
  }

  const uint32_t now = millis();
  const bool newRawPresence = digitalRead(_pin) == HIGH;

  if (newRawPresence != _rawPresence) {
    _rawPresence = newRawPresence;
    _rawChangedAt = now;
  }

  if (_rawPresence != _stablePresence &&
      (now - _rawChangedAt >= _debounceMs)) {
    _stablePresence = _rawPresence;
    return true;
  }

  return false;
}

bool LD2402PresencePin::isPresent() const {
  return _stablePresence;
}

bool LD2402PresencePin::rawPresence() const {
  return _rawPresence;
}

uint8_t LD2402PresencePin::pin() const {
  return _pin;
}
