#include <Arduino.h>
#include <RadioLib.h>
#include <Config.h>
#include <TelemetryPacket.h>

/*
 * Connections to Teensy 4.0
 * Pins:  13    14     15    18   22       24
 *        SCK   DIO1   BUSY  RST  FreqRes  3V3
 * ----------------------------------------------------------
 * ----------------------------------------------------------
 * ----------------------------------------------------------------
 * ----------- SX1262 Breakout ------------------------------------
 * ----------------------------------------------------------------
 * ----------------------------------------------------------
 * ----------------------------------------------------------
 *        RX1/TXD   TX1/RXD    NSS
 *        0         1          10
 *
 */

SX1262 radio = new Module(cs, irq, rst, gpio);
const char msg[] = "ping";

volatile bool transmissionDone = false;
volatile bool receivedPacket = false;

void isrTX()
{
    Serial.println("packet transmitted done");
    transmissionDone = true;
}
bool tx = true;

void setup()
{
    // write your initialization code here

    Serial.begin(9600);
    Serial.println("Right before radio initialize");
    int state = radio.begin(FREQUENCY_USED, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
    Serial.print("state is: ");
    Serial.println(state);
    Serial.println("Right after radio initialize");
    radio.setDio1Action(isrTX);
}

void loop()
{
    delay(1000);
    Serial.println("Looping");
    int state = radio.transmit("ping");
    Serial.println("Transmitting...");
    while (!transmissionDone)
    {
        Serial.println("Waiting for transmission...");
        Serial.println(state);
        delay(1000);
    }
    transmissionDone = false; // Reset for next transmission
}