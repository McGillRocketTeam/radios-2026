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

void setup() {

// write your initialization code here
    Console.begin();
    delay(2000);
    Console.println("starting the code");
    groundStation = GroundStation::getInstance();
    groundStation->startCommandParser();
    groundStation->startAsyncReceive();
    Console.println("Ready to receive commands");
    groundStation->setEnableTXFromCTS(false);
}
int count = 0;
void loop() {
    
    groundStation->handleRocketCommand();
    while (!groundStation->getRadioModule()->checkInterruptReceived()) {
        // Serial.println("wait" + String(count));
        // count++;
        delay(10);
    }
    Console.println("received something");
    groundStation->handleReceivedPacket();
    // if interrupt recevied run receive packet of ground station
    delay(500);
}