#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "Button.h"

class ButtonPad
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

    ButtonPad(uint8_t leftPin,
              uint8_t rightPin,
              uint8_t upPin,
              uint8_t downPin,
              uint8_t pressPin);

    void begin();
    void update();

    // Returns true only when a new event is available.
    bool getInput(Event& event);

    const char* eventToString(Event event) const;

private:
    static constexpr uint8_t EVENT_QUEUE_SIZE = 8;

    Button leftButton_;
    Button rightButton_;
    Button upButton_;
    Button downButton_;
    Button pressButton_;

    Event eventQueue_[EVENT_QUEUE_SIZE];
    uint8_t queueHead_;
    uint8_t queueTail_;
    uint8_t queueCount_;

private:
    void pushEvent(Event event);
};