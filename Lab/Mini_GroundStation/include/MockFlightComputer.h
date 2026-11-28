#pragma once

#include <Arduino.h>

class RadioReceiver;

// Port of GroundStation's FC_Mock state machine, with a polled response timeout.
class MockFlightComputer
{
public:
    void reset();
    bool poll(RadioReceiver& radio);

    uint16_t lastSequence() const { return lastSequence_; }
    uint8_t lastFlags() const { return lastFlags_; }
    uint8_t ackId() const { return ackId_; }
    const char* status() const { return status_; }

private:
    enum class State : uint8_t { NoCTS, NoCTS_NAK, CTS, Waiting, ACK };
    State state_ = State::NoCTS;
    uint16_t sequence_ = 1;
    uint16_t lastSequence_ = 0;
    uint8_t lastFlags_ = 0;
    uint8_t ackId_ = 0;
    uint32_t stateStartedMs_ = 0;
    uint32_t waitMs_ = 0;
    const char* status_ = "Ready to transmit";
};
