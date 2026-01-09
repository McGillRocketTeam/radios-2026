#include <Arduino.h>
#include <RadioLib.h>
#include <cassert>

#include "Config.h"
#include "ConsoleRouter.h"
#include "GroundStation.h"
#include "RadioModule.h"

GroundStation* groundStation;

void setup() {
    Console.begin();
    groundStation = GroundStation::getInstance();
    groundStation->initialise();
    groundStation->setCanTXFromCTS(false);
}

void loop() {
    // Delay for stability
    delay(20);

    Console.handleConsoleReconnect();
    Console.mqttLoop();
    
    // Handle received commands and packets from the ground station
    groundStation->handleCommandParserUpdate();
    groundStation->handleRadioCommand();
    groundStation->handleReceivedPacket();
}
