#pragma once

#include <Arduino.h>
#include <RadioLib.h>

#include "LoraParamConfig.h"
#include "TelemetryStore.h"
#include "peripheralPins.h"

class RadioReceiver
{
public:
    RadioReceiver();

    // Initializes the detected radio band and leaves the radio in receive mode.
    bool begin();

    // Non-blocking receive poll. Returns true when a telemetry snapshot changed.
    bool poll(TelemetryStore& telemetry);

    // Raw command reception and telemetry transmission for MockFC mode.
    bool receivePacket(const uint8_t*& data, size_t& length);
    bool transmit(const uint8_t* data, size_t length);

    bool isReady() const;

private:
    static volatile bool interruptReceived_;
    static void onDio1Interrupt();

    bool restartReceive();
    void logRadioError(const char* operation, int16_t state) const;

    Module module_;
    SX1262 radio903_;
    SX1268 radio435_;
    SX126x* radio_ = nullptr;
    uint8_t buffer_[RADIO_BUFFER_SIZE]{};
    float frequency_ = FREQUENCY_435;
    int frequencyPinValue_ = 0;
    bool bandDetected_ = false;
    bool ready_ = false;
    float lastRssi_ = 0;
    float lastSnr_ = 0;
};
