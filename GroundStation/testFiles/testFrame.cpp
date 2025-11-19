#include <Arduino.h>
#include <RadioLib.h>
#include <Config.h>
#include <cassert>
#include <RadioModule.h>
#include "GroundStation.h"
#include "frame_builder.h"
#include "frame_header.h"
#include "telemetry_packets.h"
#include "frame_view.h"
#include "frame_printer.h"
#include "ConsoleRouter.h"


GroundStation* groundStation;

void setup() {
    Console.begin();
    groundStation = GroundStation::getInstance();
    groundStation->initialise();
    
    groundStation->setEnableTXFromCTS(false);

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
