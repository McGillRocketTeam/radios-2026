#pragma once

#include <Arduino.h>

class Button {
private:
    uint8_t pin_;

    bool lastReading_;        // most recent raw read
    bool stableState_;        // debounced stable state
    unsigned long lastDebounceTime_;
    unsigned long debounceDelayMs_;

public:
    explicit Button(uint8_t pin, unsigned long debounceDelayMs = 20);

    void begin();

    // Returns true once when a debounced falling edge is detected
    bool poll();
};
