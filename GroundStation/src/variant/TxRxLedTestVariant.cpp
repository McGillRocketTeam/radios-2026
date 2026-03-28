#include "TxRxLedTestVariant.h"

#include <Arduino.h>
#include <RadioLib.h>
#include "PinLayout.h"
#include "LoraParamConfig.h"

void TxRxLedTestVariant::setup() {
    Serial.begin(115200);
    while (!Serial)
    {
    }

    Serial.println("Initializing LED pins...");
    pinMode(RX_LED_PIN, OUTPUT);
    pinMode(TX_LED_PIN, OUTPUT);
    Serial.println("LED pins initialized...");

    Serial.println("Beginning Tx/Rx LED Test...");
}

void TxRxLedTestVariant::loop() {
    digitalWrite(RX_LED_PIN, HIGH);
    digitalWrite(TX_LED_PIN, HIGH);
    delay(500);
    digitalWrite(RX_LED_PIN, LOW);
    digitalWrite(TX_LED_PIN, LOW);
    delay(500);
}