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
#include <PacketCreator.h>
#include <PacketPrinter.h>
#include <GroundStation.h>
#include <PacketController.h>

PacketController currentPacketController;
GroundStation * groundStation;


void setup() {
    groundStation = GroundStation::getInstance();
    groundStation->startCommandParser();
    Console.begin();
    delay(500);    
}


int count = 0;
void loop() {
    delay(50);
    groundStation->handleCommandParserUpdate();
    groundStation->handleRadioCommand();
    currentPacketController = PacketCreator::generateNextIncrementPacket();
    if (currentPacketController.getType() == TelemetryPacketType::PACKET_1){
        PacketPrinter::print_telemetry_packet(&currentPacketController.getPacket1());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_2){
        PacketPrinter::print_telemetry_packet(&currentPacketController.getPacket2());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_3){
        PacketPrinter::print_telemetry_packet(&currentPacketController.getPacket3());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_4){
        PacketPrinter::print_telemetry_packet(&currentPacketController.getPacket4());
    }
    else{
        Serial.println("Error when generating increment packet"); 
    }
    delay(1000);
}