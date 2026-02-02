#include <Arduino.h>

#include <cassert>

#include "Config.h"
#include "CommandParser.h"
#include "ConsoleRouter.h"
#include "MqttTopics.h"
#include "GroundStation.h"



void setup() {
    auto& cmd = CommandParser::getInstance();

    Console.begin(MqttTopic::Role::CS,cmd);

    auto& gs = GroundStation::getInstance();
    gs.initialise(cmd);
    gs.setCanTXFromCTS(true);

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
