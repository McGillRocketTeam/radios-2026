#include <Arduino.h>

#include <cassert>

#include "GroundStationVariant.h"
#include "Config.h"
#include "CommandParser.h"
#include "ConsoleRouter.h"
#include "MqttTopics.h"
#include "GroundStation.h"

// Variant for full ground station radio operation
void GroundStationVariant::setup() {
    auto& cmd = CommandParser::getInstance();
    auto role = MqttTopic::Role::CS;
    #ifdef DEVICE_VARIANT_GROUNDSTATION
        #ifdef GROUNDSTATION_LOCATION_CONTROLSTATION
            role = MqttTopic::Role::CS;
        #elif defined(GROUNDSTATION_LOCATION_PAD)
            role = MqttTopic::Role::PD;
        #else
            #error "No valid ground station location selected"
        #endif
    #endif

    Console.begin(role,cmd);

    auto& gs = GroundStation::getInstance();
    gs.initialise(cmd);
    gs.setCanTXFromCTS(true);

}

void GroundStationVariant::loop() {
    delay(1);

    Console.handleConsoleReconnect();
    Console.mqttLoop();

    auto& gs = GroundStation::getInstance();

    gs.handleCommandParserUpdate();
    gs.handleGroundCommand();
    gs.handleReceivedPacket();
}
