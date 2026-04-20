#include "ButtonPad.h"

ButtonPad::ButtonPad(uint8_t leftPin,
                     uint8_t rightPin,
                     uint8_t upPin,
                     uint8_t downPin,
                     uint8_t pressPin)
    : leftButton_(leftPin),
      rightButton_(rightPin),
      upButton_(upPin),
      downButton_(downPin),
      pressButton_(pressPin),
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
    pressButton_.begin();

    queueHead_ = 0;
    queueTail_ = 0;
    queueCount_ = 0;
}

void ButtonPad::update()
{
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

    if (pressButton_.poll())
    {
        pushEvent(Event::PRESSED);
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
        case Event::PRESSED: return "PRESSED";
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