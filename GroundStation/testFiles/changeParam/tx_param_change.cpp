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


telemetry_packet_1 packet1;
GroundStation * groundStation;

void setup() {

// write your initialization code here
    Serial.begin(9600);
    delay(2000);
    Serial.println("starting the code");
    groundStation = GroundStation::getInstance();
    groundStation->getRadioModule()->setBandwidth(125);
    groundStation->startCommandParser();
    groundStation->startAsyncReceive();
    groundStation->setEnableTXFromCTS(false);
    Serial.println("Ready to receive commands");

}
int count = 0;
void loop() {
    count++;
    groundStation->handleCommandParserUpdate();
    groundStation->handleRadioCommand();
    delay(150);
    packet1 = PacketCreator::generateIncrementPacket<telemetry_packet_1>();
    groundStation->getRadioModule()->transmitInterrupt(packet1.bytes, sizeof(packet1));
}