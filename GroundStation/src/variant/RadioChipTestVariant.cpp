#include "RadioChipTestVariant.h"

#include <Arduino.h>
#include <RadioLib.h>
#include "ParamStore.h"
#include "RadioModule.h"
#include "PinLayout.h"
#include "LoraParamConfig.h"

void RadioChipTestVariant::setup()
{
    // Waiting for connection
    Serial.begin(115200);
    while(!Serial) {}

    // 1. Deducing the frequency band based on the hardware pin
    bool is903_pin = ParamStore::is903();
    bool is435 = !is903_pin;
    
    // Deducing the frequency band 
    Serial.println("1. Deducing Radio Frequency Band from pin");
    Serial.print(ParamStore::getDefaultBandFreq());
    Serial.println(" MHz");

    if (is903_pin) {
        Serial.println("Band Deduced from pin: 900 (Uses SX1262)");
    } else if (is435) {
        Serial.println("Band Deduced from pin: 400 (Uses SX1268)");
    } else {
        Serial.println("Band Deduced from pin: UNKNOWN ERROR");
    }

    Serial.println();
    
    // 2. Probing the actual chip using RadioLib directly
    Serial.println("2. Probing Physical Chip via SPI");
    Module mod(NSS_PIN, DIO1_PIN, RST_PIN, BUSY_PIN);
    SX1262 probe1262(&mod);
    SX1268 probe1268(&mod);

    // Try starting it as a 900 MHz SX1262
    int state1262 = probe1262.begin(FREQUENCY_903, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
    probe1262.standby();

    // Try starting it as a 400 MHz SX1268
    int state1268 = probe1268.begin(FREQUENCY_435, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
    probe1268.standby();

    // Checking what chip we are actually working with
    bool chip_is_1262 = (state1262 == RADIOLIB_ERR_NONE);
    bool chip_is_1268 = (state1268 == RADIOLIB_ERR_NONE);

    if (chip_is_1262 && chip_is_1268) {
        Serial.println("WARNING: Both initializations succeeded. Hardware might be ambiguous or bypassing hardware checks.");
    } else if (!chip_is_1262 && !chip_is_1268) {
        Serial.println("FATAL: Both initializations failed. The SPI connection is likely broken or the chip is dead.");
    } else {
        bool actual_is_903 = chip_is_1262;

        Serial.println("Chip deduced: " + String(actual_is_903 ? "SX1262" : "SX1268"));
        
        if (actual_is_903 == is903_pin) {
            Serial.println("Frequency Test: SUCCESS. The chip matches the configuration of FREQ_PIN.");
        } else {
            Serial.println("Frequency Test: FAILURE. The chip does NOT match the configuration of FREQ_PIN.");
            Serial.println("Aborting test...");
            return;
        }

        Serial.println("4. Testing State Transitions (Standby, TX, RX)");
        // Inherit base SX126x class to test the detected hardware
        SX126x* active_radio = actual_is_903 ? (SX126x*)&probe1262 : (SX126x*)&probe1268;
        
        // Re-initialize again
        if (actual_is_903) {
            probe1262.begin(FREQUENCY_903, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
        } else {
            probe1268.begin(FREQUENCY_435, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
        }

        // Test Standby
        Serial.print("Testing Standby Mode... ");
        int standbyState = active_radio->standby();

        // Test Blocking Transmit
        Serial.print("Testing Transmit Mode (Sending 'MRT_TEST')... ");
        uint8_t testMsg[] = "MRT_TEST";
        int txState = active_radio->transmit(testMsg, sizeof(testMsg));

        // Test Asynchronous Receive Initialization
        Serial.print("Testing Receive Mode... ");
        int rxState = active_radio->startReceive();
        
        active_radio->standby(); // return the radio to a safe idle state

        Serial.println("5. Summary of State Transition Tests:");
        Serial.println("Standby State: " + String(standbyState == RADIOLIB_ERR_NONE ? "SUCCESS" : "FAILURE"));
        Serial.println("Transmit State: " + String(txState == RADIOLIB_ERR_NONE ? "SUCCESS" : "FAILURE"));
        Serial.println("Receive State: " + String(rxState == RADIOLIB_ERR_NONE ? "SUCCESS" : "FAILURE"));
    }

    Serial.println("================================================");
}

void RadioChipTestVariant::loop()
{
    // loop forever
    delay(1000);
}