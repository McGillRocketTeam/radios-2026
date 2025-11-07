#include <Arduino.h>
#include <RadioLib.h>
#include <Config.h>
#include <cassert>
#include <RadioModule.h>
#include <BooleanDataPacket.h>
#include <PayloadDataPacket.h>
#include <FlightComputerDataPacket.h>
#include <RadioDataPacket.h>
#include <TelemetryPacket.h>
#include "GroundStation.h"
#include <PacketCreator.h>
#include <PacketPrinter.h>

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

telemetry_packet_1 packet1;
telemetry_packet_2 packet2;
telemetry_packet_3 packet3;
telemetry_packet_4 packet4;
telemetry_packet_1 def;
GroundStation * groundStation;
int count;
void setup() {

// write your initialization code here
    Serial.begin(9600);
    delay(2000);
    Serial.println("starting the code");
    groundStation = GroundStation::getInstance();
    groundStation->startCommandParser();
    groundStation->startAsyncReceive();
    Serial.println("Ready to receive commands");
    // groundStation->getRadioModule()->setBandwidth(125);
    groundStation->setEnableTXFromCTS(false);

}
void loop() {
    groundStation->handleCommandParserUpdate();
    groundStation->handleRadioCommand();
    // groundStation->handleRocketCommand();
    groundStation->handleReceivedPacket();
    count++;
    if (count % 100 == 0){
        groundStation->handleRocketCommand();
    }
    delay(10);
}