#include "TestVariant.h"
#include <Arduino.h>
#include <RadioLib.h>
#include "PinLayout.h"
#include "LoraParamConfig.h"

SX1262 radio = new Module(NSS_PIN, DIO1_PIN, RST_PIN, BUSY_PIN);

void TestVariant::setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
    }

    Serial.println("Initializing SX1262...");

    int state = radio.begin(
        FREQUENCY_903, 
        BANDWIDTH_USED,
        SPREADING_FACTOR_USED,
        CODING_RATE_USED,
        SYNC_WORD,
        POWER_OUTPUT,
        PREAMBLE_LENGTH,
        TCXO_VOLTAGE,
        USE_ONLY_LDO
    );

    if (state != RADIOLIB_ERR_NONE)
    {
        Serial.print("Init failed, code: ");
        Serial.println(state);
        while (true)
        {
        }
    }

    Serial.println("SX1262 init OK");
}

void TestVariant::loop()
{
    radio.transmit("hi");
    Serial.println("TX completed");
    delay(1000);
}