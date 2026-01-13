#include <Arduino.h>

#include <cassert>

#include "Config.h"
#include "ConsoleRouter.h"
#include "GroundStation.h"

// ======================================================== 
// Variant of main.cpp that runs the main GS radio code
// 

void setup() {
    Console.begin();

    auto& gs = GroundStation::getInstance();
    gs.initialise();
    gs.setCanTXFromCTS(false);

}

void loop() {
    delay(20);

    Console.handleConsoleReconnect();
    Console.mqttLoop();

    auto& gs = GroundStation::getInstance();

    // Handle received commands and packets from the ground station
    gs.handleCommandParserUpdate();
    gs.handleRadioCommand();
    gs.handleReceivedPacket();
}
