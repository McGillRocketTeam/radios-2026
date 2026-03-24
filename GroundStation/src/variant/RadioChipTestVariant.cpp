#include "RadioChipTestVariant.h"

#include <Arduino.h>
#include <RadioLib.h>
#include "ParamStore.h"
#include "PinLayout.h"
#include "LoraParamConfig.h"

void RadioChipTestVariant::setup()
{
    // Waiting for connection
    Serial.begin(115200);
    while (!Serial)
    {
    }

    // 1. Deducing the frequency band based on the hardware pin
    bool is903_pin = ParamStore::is903();
    bool is435 = !is903_pin;

    // Deducing the frequency band
    Serial.println("1. Deducing Radio Frequency Band from pin");
    Serial.print(ParamStore::getDefaultBandFreq());
    Serial.println(" MHz");

    Serial.print("Band pin Voltage ");
    Serial.print((ParamStore::getFreqPinAnalogValue())/1024);
    


    if (is903_pin)
    {
        Serial.println("Band Deduced from pin: 900 (Uses SX1262)");
    }
    else if (is435)
    {
        Serial.println("Band Deduced from pin: 400 (Uses SX1268)");
    }
    else
    {
        Serial.println("Band Deduced from pin: UNKNOWN ERROR");
    }

    Serial.println();

    // 2. Probing the actual chip using RadioLib directly
    Serial.println("2. Probing Physical Chip via SPI");
    Module mod(NSS_PIN, DIO1_PIN, RST_PIN, BUSY_PIN);
    SX1262 probe1262(&mod);
    SX1268 probe1268(&mod);

    int chip_state;
    // We strictly use the pin to decide which chip to test
    if (is903_pin)
    {
        Serial.println("FREQ_PIN says 900. Attempting to initialize as 900 MHz (SX1262)...");
        chip_state = probe1262.begin(FREQUENCY_903, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
        Serial.print("SX1262 State: ");
        Serial.println(chip_state);
        if (chip_state == RADIOLIB_ERR_NONE)
        {
            probe1262.standby();
        }
    }
    else
    {
        Serial.println("FREQ_PIN says 400. Attempting to initialize as 433 MHz (SX1268)...");
        chip_state = probe1268.begin(FREQUENCY_435, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
        Serial.print("SX1268 State: ");
        Serial.println(chip_state);
        probe1268.standby();
    }

    if (chip_state != RADIOLIB_ERR_NONE)
    {
        Serial.print("FATAL: Expected chip failed to initialize with error code: ");
        Serial.println(chip_state);

        if (chip_state == RADIOLIB_ERR_CHIP_NOT_FOUND)
        {
            Serial.println("Radio chip was not found during initialization. This "
                           "can be caused by specifying wrong chip type in the constructor ");
        }
        else if (chip_state == RADIOLIB_ERR_INVALID_FREQUENCY)
        {
            Serial.println("The supplied frequency value is invalid for this module.");
        }
        else if (chip_state == RADIOLIB_ERR_WRONG_MODEM)
        {
            Serial.println("User tried to execute modem-exclusive method on a wrong modem. "
                           "For example, this can happen when you try to change LoRa configuration when FSK modem is active.");
        }
        else if (chip_state == RADIOLIB_ERR_SPI_CMD_TIMEOUT)
        {
            Serial.println("SX126x timed out while waiting for complete SPI command.");
        }
        else
        {
            Serial.println("Check https://jgromes.github.io/RadioLib/group__status__codes.html for more details.");
        }

        Serial.println("================================================");
        return;
    }
    else
    {
        Serial.println("Frequency Test: SUCCESS. The physical chip perfectly MATCHES the FREQ_PIN configuration!");

        Serial.println("4. Testing State Transitions (Standby, TX, RX)");
        // Inherit base SX126x class to test the detected hardware
        SX126x *active_radio = is903_pin ? (SX126x *)&probe1262 : (SX126x *)&probe1268;

        // Re-initialize again
        if (is903_pin)
        {
            probe1262.begin(FREQUENCY_903, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
        }
        else
        {
            probe1268.begin(FREQUENCY_435, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
        }

        // Test Standby
        Serial.println("Testing Standby Mode... ");
        int standbyState = active_radio->standby();

        // Test Blocking Transmit
        Serial.println("Testing Transmit Mode (Sending 'MRT_TEST')... ");
        uint8_t testMsg[] = "MRT_TEST";
        int txState = active_radio->transmit(testMsg, sizeof(testMsg));
        Serial.print("TXState ");
        Serial.println(txState);

        // Test Asynchronous Receive Initialization
        Serial.println("Testing Receive Mode... ");
        int rxState = active_radio->startReceive();

        // Attempt to return the radio to a safe idle state
        active_radio->standby();

        Serial.println("5. Summary of State Transition Tests:");
        Serial.println("Standby State: " + String(standbyState == RADIOLIB_ERR_NONE ? "SUCCESS" : "FAILURE"));
        Serial.println("Transmit State: " + String(txState == RADIOLIB_ERR_NONE ? "SUCCESS" : "FAILURE"));
        Serial.println("Receive State: " + String(rxState == RADIOLIB_ERR_NONE ? "SUCCESS" : "FAILURE"));
    }

    Serial.println("================================================");
}

void RadioChipTestVariant::loop()
{
    // All work is done in setup
    delay(1000);
}