#ifndef RADIOMODULE_H
#define RADIOMODULE_H

#include <RadioLib.h>
#include <ArduinoQueue.h>
#include <RadioParams.h>
#include <PinLayout.h>
#include <RadioChip.h>
#include <LoggerGS.h>
#include <ConsoleRouter.h>

class RadioModule {
private:
    static volatile bool interruptReceived;
    static volatile bool enableInterrupt;
    
    static void radioReceiveISR(); 

    int state = RADIOLIB_ERR_NONE;
    RadioChip radio;

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

public:
    RadioModule();

    // BLOCKING Transmit data array of bytes and stay in transmit mode
    bool transmitInterrupt(const uint8_t* data, size_t size);

    bool receiveMode();
    uint8_t* readPacket();
    bool checkInterruptReceived();
    int getPacketLength();

    float getFreq();
    float getBandwidth();
    int getSpreadingFactor();
    int getCodingRate();
    int getPowerOutput();

    void setFreq(float newFrequency);
    void setBandwidth(float newBandwidth);
    void setSpreadingFactor(int newSpreadingFactor);
    void setCodingRate(int newCodingRate);
    void setPowerOutput(int newPowerOutput);

    void checkParams();
    void pingParams();

    RadioChip* getRadioChipInstance();

    // Get the RSSI of the last packet received
    float getRSSI();
    
    // Get the SNR of the last packet received
    float getSNR();
};

#endif // RADIOMODULE_H
