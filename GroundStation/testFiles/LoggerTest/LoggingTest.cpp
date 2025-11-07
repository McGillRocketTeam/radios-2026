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


telemetry_packet_1 packet;
GroundStation * groundStation;

void setup() {

    Serial.begin(9600);
    delay(2000);
    groundStation = GroundStation::getInstance();
    groundStation->initialise();
    //CTS stuff
    groundStation->setEnableTXFromCTS(true);
    
}
void loop() {
    LOGGING("Radio","hello");
    LOGGING("Radio",2);
    LOGGING("Parser",3.1f);
    LOGGING("Verbose",3.2f);
    delay(1000);
}