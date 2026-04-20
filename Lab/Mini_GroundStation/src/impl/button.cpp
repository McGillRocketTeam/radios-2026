#include "Button.h"

Button::Button(uint8_t pin, unsigned long debounceDelayMs)
    : pin_(pin),
      lastReading_(HIGH),
      stableState_(HIGH),
      lastDebounceTime_(0),
      debounceDelayMs_(debounceDelayMs) {}

void Button::begin() {
    pinMode(pin_, INPUT_PULLUP);

    bool initialState = digitalRead(pin_);
    lastReading_ = initialState;
    stableState_ = initialState;
    lastDebounceTime_ = millis();
}

bool Button::poll() {
    bool reading = digitalRead(pin_);

    if (reading != lastReading_) {
        lastDebounceTime_ = millis();
        lastReading_ = reading;
    }

    if ((millis() - lastDebounceTime_) >= debounceDelayMs_) {
        if (stableState_ != reading) {
            bool previousStableState = stableState_;
            stableState_ = reading;

            if (previousStableState == HIGH && stableState_ == LOW) {
                return true;
            }
        }
    }

    return false;
}