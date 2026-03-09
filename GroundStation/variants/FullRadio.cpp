#include <Arduino.h>

#include <cassert>

#include "Config.h"
#include "CommandParser.h"
#include "ConsoleRouter.h"
#include "MqttTopics.h"
#include "GroundStation.h"

// Variant for full ground station radio operation
// TODO make a env flag centered main dispatcher
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

    gs.handleCommandParserUpdate();
    gs.handleGroundCommand();
    gs.handleReceivedPacket();
}
