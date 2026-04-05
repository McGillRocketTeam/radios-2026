#include "Joystick.h"

Joystick::Joystick(uint8_t vrxPin, uint8_t vryPin, uint8_t swPin)
    : vrxPin_(vrxPin),
      vryPin_(vryPin),
      swPin_(swPin),
      stableDirectionEvent_(Event::NONE),
      tentativeDirectionEvent_(Event::NONE),
      stablePressed_(false),
      tentativePressed_(false),
      directionCandidateStartMs_(0),
      buttonCandidateStartMs_(0),
      directionCandidateActive_(false),
      buttonCandidateActive_(false),
      pendingEvent_(false),
      pendingInputEvent_(Event::NONE)
{
}

void Joystick::begin()
{
    pinMode(vrxPin_, INPUT);
    pinMode(vryPin_, INPUT);
    pinMode(swPin_, INPUT_PULLUP);

    stableDirectionEvent_ = readRawDirectionEvent();
    tentativeDirectionEvent_ = stableDirectionEvent_;

    stablePressed_ = readRawPressed();
    tentativePressed_ = stablePressed_;

    directionCandidateStartMs_ = 0;
    buttonCandidateStartMs_ = 0;

    directionCandidateActive_ = false;
    buttonCandidateActive_ = false;

    pendingEvent_ = false;
    pendingInputEvent_ = Event::NONE;
}

void Joystick::update()
{
    const unsigned long now = millis();

    Event rawDirectionEvent = readRawDirectionEvent();

    if (rawDirectionEvent == stableDirectionEvent_)
    {
        directionCandidateActive_ = false;
    }
    else
    {
        if (!directionCandidateActive_)
        {
            tentativeDirectionEvent_ = rawDirectionEvent;
            directionCandidateStartMs_ = now;
            directionCandidateActive_ = true;
        }
        else if (rawDirectionEvent != tentativeDirectionEvent_)
        {
            tentativeDirectionEvent_ = rawDirectionEvent;
            directionCandidateStartMs_ = now;
        }
        else if ((now - directionCandidateStartMs_) >= DEBOUNCE_MS)
        {
            stableDirectionEvent_ = tentativeDirectionEvent_;
            directionCandidateActive_ = false;

            if (stableDirectionEvent_ != Event::NONE && !pendingEvent_)
            {
                pendingInputEvent_ = stableDirectionEvent_;
                pendingEvent_ = true;
            }
        }
    }

    bool rawPressed = readRawPressed();

    if (rawPressed == stablePressed_)
    {
        buttonCandidateActive_ = false;
    }
    else
    {
        if (!buttonCandidateActive_)
        {
            tentativePressed_ = rawPressed;
            buttonCandidateStartMs_ = now;
            buttonCandidateActive_ = true;
        }
        else if (rawPressed != tentativePressed_)
        {
            tentativePressed_ = rawPressed;
            buttonCandidateStartMs_ = now;
        }
        else if ((now - buttonCandidateStartMs_) >= DEBOUNCE_MS)
        {
            stablePressed_ = tentativePressed_;
            buttonCandidateActive_ = false;

            if (stablePressed_ && !pendingEvent_)
            {
                pendingInputEvent_ = Event::PRESSED;
                pendingEvent_ = true;
            }
        }
    }
}

bool Joystick::getInput(Event& event)
{
    if (!pendingEvent_)
    {
        return false;
    }

    event = pendingInputEvent_;
    pendingInputEvent_ = Event::NONE;
    pendingEvent_ = false;
    return true;
}

Joystick::Event Joystick::readRawDirectionEvent() const
{
    const int vrx = analogRead(vrxPin_);
    const int vry = analogRead(vryPin_);

    if (vrx < VRX_LEFT_THRESHOLD)
    {
        return Event::LEFT;
    }

    if (vrx > VRX_RIGHT_THRESHOLD)
    {
        return Event::RIGHT;
    }

    if (vry < VRY_UP_THRESHOLD)
    {
        return Event::UP;
    }

    if (vry > VRY_DOWN_THRESHOLD)
    {
        return Event::DOWN;
    }

    return Event::NONE;
}

bool Joystick::readRawPressed() const
{
    return (digitalRead(swPin_) == LOW);
}

const char* Joystick::eventToString(Joystick::Event event)
{
    switch (event)
    {
        case Joystick::Event::LEFT:   return "LEFT";
        case Joystick::Event::RIGHT:  return "RIGHT";
        case Joystick::Event::UP:     return "UP";
        case Joystick::Event::DOWN:   return "DOWN";
        case Joystick::Event::NONE:    return "MIDDLE";
        case Joystick::Event::PRESSED: return "PRESSED";
        default:                          return "?";
    }
}