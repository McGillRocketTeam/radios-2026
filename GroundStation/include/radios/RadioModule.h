#pragma once

#include <RadioLib.h>

#include "ConsoleRouter.h"
#include "LoggerGS.h"
#include "PinLayout.h"
#include "RadioChip.h"

class RadioModule {
private:
    static volatile bool interruptReceived;
    static volatile bool radioBusy;
    static void radioDio1ISR(); 

    RadioChipStatus state_ = RADIOLIB_ERR_NONE;
    Module mod_;
    RadioChip radio_;

    // RX_LED and TX_LED toggle when each of the ops is done
    int rxLedPin_ = RX_LED_PIN;
    int txLedPin_ = TX_LED_PIN;
    bool rxLedState_ = false;
    bool txLedState_ = false;

    // TODO we should defer to paramstore every time to get params
    // What about in the situations where we want to run special stuff? 
    // We need a smart way to overload paramstore
    uint8_t buffer[RADIO_BUFFER_SIZE];
    int lastPacketLength = 0;
    
    // Checks that the current state of the chip is good
    // If its bad then we will output the critical messsage
    bool verifyRadioState(String message);

    void toggleLedOnOk(int pin);

public:
    RadioModule();

    bool retryRadioInit();

    // Blocking transmit a block and switch into receive mode
    bool transmitBlocking(const uint8_t* data, size_t size);

    bool receiveMode();
    // Poll for a perfect rx, always puts radio back to ready receive mode
    // on success saves data into local buffer, fail will log why
    bool pollValidPacketRx();

    uint8_t* getPacketData();
    int getPacketLength();

    float getFreq();
    float getBandwidth();
    int getSpreadingFactor();
    int getCodingRate();
    int getPowerOutput();

    bool setFreq(float newFrequency);
    bool setBandwidth(float newBandwidth);
    bool setSpreadingFactor(uint8_t newSpreadingFactor);
    bool setCodingRate(uint8_t newCodingRate);
    bool setPowerOutput(int8_t newPowerOutput);

    void checkParams();
    void pingParams();

    RadioChip* getRadioChipInstance();

    // Get the RSSI of the last packet received
    float getRSSI();

    // Get the raw RSSI register value of last packet received
    uint8_t getRawRSSI();
    
    // Get the SNR of the last packet received
    float getSNR();
    
    // Get the raw SNR register value of last packet received
    int8_t getRawSNR();
};

