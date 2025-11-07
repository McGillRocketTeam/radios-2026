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
#include <ConsoleRouter.h>


GroundStation * groundStation;

void setup() {

    Console.begin();
    groundStation = GroundStation::getInstance();
    groundStation->initialise();
    //CTS stuff
    groundStation->setEnableTXFromCTS(true);
}
void loop() {
    //Delay for stabillity
    delay(20);
    // if interrupt recevied run receive packet of ground station
    groundStation->handleCommandParserUpdate();
    Console.handleConsoleReconnect();
    groundStation->handleRadioCommand();
    groundStation->handleReceivedPacket();
    // groundStation->handleRocketCommand();
}