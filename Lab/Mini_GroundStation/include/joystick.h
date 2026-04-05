#pragma once

#include <Arduino.h>
#include <stdint.h>

class Joystick
{
public:
    enum class Event : uint8_t
    {
        NONE,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        PRESSED
    };

    Joystick(uint8_t vrxPin, uint8_t vryPin, uint8_t swPin);

    void begin();
    void update();

    // Returns true only when a new stable event is available.
    bool getInput(Event& event);

    const char* eventToString(Event event);

private:
    static constexpr uint16_t DEBOUNCE_MS = 50;

    static constexpr int VRX_LEFT_THRESHOLD  = 100;
    static constexpr int VRX_RIGHT_THRESHOLD = 900;
    static constexpr int VRY_UP_THRESHOLD    = 100;
    static constexpr int VRY_DOWN_THRESHOLD  = 900;

    uint8_t vrxPin_;
    uint8_t vryPin_;
    uint8_t swPin_;

    Event stableDirectionEvent_;
    Event tentativeDirectionEvent_;

    bool stablePressed_;
    bool tentativePressed_;

    unsigned long directionCandidateStartMs_;
    unsigned long buttonCandidateStartMs_;

    bool directionCandidateActive_;
    bool buttonCandidateActive_;

    bool pendingEvent_;
    Event pendingInputEvent_;

private:
    Event readRawDirectionEvent() const;
    bool readRawPressed() const;
};