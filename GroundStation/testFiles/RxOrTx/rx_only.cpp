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
#include <LoggerGS.h>

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

telemetry_packet_1 packet;
GroundStation * groundStation;

void setup() {

// write your initialization code here
    Console.begin();
    delay(2000);
    LOGGING(DEBUG_GS,"starting the code");
    groundStation = GroundStation::getInstance();
    groundStation->startCommandParser();
    groundStation->startAsyncReceive();
    Console.println("Ready to receive commands");
    groundStation->setEnableTXFromCTS(false);
}
int count = 0;
void loop() {
    
    // if interrupt recevied run receive packet of ground station
    Console.handleConsoleReconnect();
    groundStation->handleCommandParserUpdate();
    groundStation->handleRadioCommand();
    groundStation->handleReceivedPacket();
    delay(10);
}