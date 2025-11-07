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


telemetry_packet_1 packet;
GroundStation * groundStation;
/*

MAKE SURE TABBED BOOLEANS IS TRUE
MAKE SURE GUI PRINT IS FALSE
OPTIONAL ENABLE TX FROM CTS, CONTROLE WHETHER RADIO SENDS COMMANDS
*/
void setup() {

    Serial.begin(9600);
    delay(2000);
    groundStation = GroundStation::getInstance();
    groundStation->initialise();
    //CTS stuff
    groundStation->setEnableTXFromCTS(false);
}
void loop() {
    // if interrupt recevied run receive packet of ground station
    groundStation->handleCommandParserUpdate();
    groundStation->handleRadioCommand();
    groundStation->handleReceivedPacket();
    // groundStation->handleRocketCommand();
}