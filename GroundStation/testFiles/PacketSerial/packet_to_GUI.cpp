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
    delay(2000);    
}


void loop() {
    delay(100);
    Console.handleConsoleReconnect();
    groundStation->handleCommandParserUpdate();
    groundStation->handleRadioCommand();
    currentPacketController = PacketCreator::generateNextIncrementPacket();
    currentPacketController.setLocalRadioData(21,59);
    if (currentPacketController.getType() == TelemetryPacketType::PACKET_1){
        PacketPrinter::packet_to_serial_variable_length(&currentPacketController.getPacket1(),&currentPacketController);
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_2){
        PacketPrinter::packet_to_serial_variable_length(&currentPacketController.getPacket2(),&currentPacketController);
    }
    if (currentPacketController.getType() == TelemetryPacketType::PACKET_3){
        PacketPrinter::packet_to_serial_variable_length(&currentPacketController.getPacket3(),&currentPacketController);
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_4){
        PacketPrinter::packet_to_serial_variable_length(&currentPacketController.getPacket4(),&currentPacketController);
    }
    else{
        // Serial.println("Error when generating increment packet"); 
    }
}