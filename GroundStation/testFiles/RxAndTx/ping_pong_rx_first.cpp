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
telemetry_packet_2 packet2;
telemetry_packet_3 packet3;
telemetry_packet_4 packet4;
telemetry_packet_1 def;
GroundStation * groundStation;
PacketController currentPacketController;
RadioModule * radioModule;

void setup() {

// write your initialization code here
    Console.begin();
    delay(2000);
    Console.println("starting the coede");
    groundStation = GroundStation::getInstance();
    groundStation->startCommandParser();
    groundStation->startAsyncReceive();
    groundStation->setEnableTXFromCTS(false);
    Console.println("Ready to receive commands");

}
int count = 0;
void loop() {
    count++;
    Console.println("starting loop");
    groundStation->startAsyncReceive();
    while (!groundStation->getRadioModule()->checkInterruptReceived()) {
        delay(10);
    }
    Console.println("received something");
    groundStation->handleReceivedPacket();
    // if interrupt recevied run receive packet of ground station
    delay(500);
    int value = count % 4;
    currentPacketController = PacketCreator::generateNextIncrementPacket();
    radioModule = groundStation->getRadioModule();
    if (currentPacketController.getType() == TelemetryPacketType::PACKET_1){
        LOGGING(DEBUG_RADIO,"Sending packet 1 ");
        radioModule->transmitInterrupt(currentPacketController.getPacket1().bytes,currentPacketController.getPacketSize());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_2){
        LOGGING(DEBUG_RADIO,"Sending packet 2");
        radioModule->transmitInterrupt(currentPacketController.getPacket1().bytes,currentPacketController.getPacketSize());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_3){
        LOGGING(DEBUG_RADIO,"Sending packet 3");
        radioModule->transmitInterrupt(currentPacketController.getPacket1().bytes,currentPacketController.getPacketSize());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_4){
        LOGGING(DEBUG_RADIO,"Sending packet 4");
        radioModule->transmitInterrupt(currentPacketController.getPacket1().bytes,currentPacketController.getPacketSize());
    }
    else{
        Console.println("Error when generating increment packet"); 
    }
}