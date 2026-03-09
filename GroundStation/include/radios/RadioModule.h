#pragma once

#include <ArduinoQueue.h>
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

    float frequency;
    float bandwidth = BANDWIDTH_USED;
    int spreadingFactor = SPREADING_FACTOR_USED;
    int codingRate = CODING_RATE_USED;
    int syncWord = SYNC_WORD;
    int powerOutput = POWER_OUTPUT;
    int preambleLength = PREAMBLE_LENGTH;
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

    void setFreq(float newFrequency);
    void setBandwidth(float newBandwidth);
    void setSpreadingFactor(uint8_t newSpreadingFactor);
    void setCodingRate(uint8_t newCodingRate);
    void setPowerOutput(int8_t newPowerOutput);

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

