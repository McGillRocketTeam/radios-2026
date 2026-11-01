#include "ButtonPad.h"

ButtonPad::ButtonPad(uint8_t leftPin,
                     uint8_t rightPin,
                     uint8_t upPin,
                     uint8_t downPin,
                     uint8_t middlePin)
    : leftButton_(leftPin),
      rightButton_(rightPin),
      upButton_(upPin),
      downButton_(downPin),
      middleButton_(middlePin),
      queueHead_(0),
      queueTail_(0),
      queueCount_(0)
{
}

void ButtonPad::begin()
{
    leftButton_.begin();
    rightButton_.begin();
    upButton_.begin();
    downButton_.begin();
    middleButton_.begin();

    queueHead_ = 0;
    queueTail_ = 0;
    queueCount_ = 0;

    middleWasPressed_ = middleButton_.isPressed();
    middleLongPressSent_ = false;
    middlePressStartTimeMS_ = millis();
}

void ButtonPad::update()
{
    // Special handling for the middle button
    middleButton_.poll();

    const bool pressed = middleButton_.isPressed();
    const unsigned long now = millis();

    if  (pressed && !middleWasPressed_) // Pressed and was not pressed before
    {
        middlePressStartTimeMS_ = now;
        middleLongPressSent_ = false;
    }
    else if (pressed && !middleLongPressSent_) // Still pressed and long press not sent yet
    {
        if (now - middlePressStartTimeMS_ >= LONG_PRESS_THRESHOLD_MS)
        {
            pushEvent(Event::MIDDLE_LONG_PRESSED);
            middleLongPressSent_ = true;
        }
    }
    else if (!pressed && middleWasPressed_) // Released and was pressed before
    {
        if (!middleLongPressSent_)
        {
            pushEvent(Event::MIDDLE);
        }
    }

    middleWasPressed_ = pressed;

    // Simple logic for other buttons
    if (leftButton_.poll())
    {
        pushEvent(Event::LEFT);
    }

    if (rightButton_.poll())
    {
        pushEvent(Event::RIGHT);
    }

    if (upButton_.poll())
    {
        pushEvent(Event::UP);
    }

    if (downButton_.poll())
    {
        pushEvent(Event::DOWN);
    }
}

bool ButtonPad::getInput(Event& event)
{
    if (queueCount_ == 0)
    {
        event = Event::NONE;
        return false;
    }

    event = eventQueue_[queueHead_];
    queueHead_ = (queueHead_ + 1) % EVENT_QUEUE_SIZE;
    queueCount_--;

    return true;
}

const char* ButtonPad::eventToString(Event event) const
{
    switch (event)
    {
        case Event::LEFT:    return "LEFT";
        case Event::RIGHT:   return "RIGHT";
        case Event::UP:      return "UP";
        case Event::DOWN:    return "DOWN";
        case Event::MIDDLE: return "PRESSED";
        case Event::MIDDLE_LONG_PRESSED: return "LONG_PRESSED";
        default:             return "NONE";
    }
}

void ButtonPad::pushEvent(Event event)
{
    if (queueCount_ >= EVENT_QUEUE_SIZE)
    {
        return;
    }

    eventQueue_[queueTail_] = event;
    queueTail_ = (queueTail_ + 1) % EVENT_QUEUE_SIZE;
    queueCount_++;
}