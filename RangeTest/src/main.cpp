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
    const uint32_t start = millis();
    while (!Serial && (millis() - start < 300))
    {
    }

    auto& gs = GroundStation::getInstance();
    gs.initialise();
    gs.setCanTXFromCTS(true);
}

void loop() {
    delay(20);

    auto& gs = GroundStation::getInstance();

    // Handle received commands and packets from the ground station
    gs.handleCommandParserUpdate();
    gs.handleRadioCommand();
    gs.handleReceivedPacket();
}
