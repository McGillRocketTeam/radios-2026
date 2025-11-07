#include <Arduino.h>
#include <RadioLib.h>
#include <Config.h>
#include <cassert>
#include <FlightComputerDataPacket.h>
#include <RadioDataPacket.h>
#include <TelemetryPacket.h>
#include <PacketCreator.h>
#include <PacketPrinter.h>
#include <GroundStation.h>
#include <PacketController.h>
#include <LoggerGS.h>
#include <ConsoleRouter.h>

PacketController currentPacketController;
GroundStation * groundStation;
RadioModule * radioModule;

void setup() {
    groundStation = GroundStation::getInstance();
    groundStation->startCommandParser();
    Console.begin();
    delay(2000);    
}

int count = 0;
void loop() {
    delay(200);
    groundStation->handleCommandParserUpdate();
    groundStation->handleRadioCommand();
    Console.handleConsoleReconnect();
    currentPacketController = PacketCreator::generateNextIncrementPacket();
    radioModule = groundStation->getRadioModule();
    if (currentPacketController.getType() == TelemetryPacketType::PACKET_1){
        LOGGING(DEBUG_GS,"Sending packet 1 ");
        radioModule->transmitInterrupt(currentPacketController.getPacket1().bytes,currentPacketController.getPacketSize());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_2){
        LOGGING(DEBUG_GS,"Sending packet 2");
        radioModule->transmitInterrupt(currentPacketController.getPacket2().bytes,currentPacketController.getPacketSize());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_3){
        LOGGING(DEBUG_GS,"Sending packet 3");
        radioModule->transmitInterrupt(currentPacketController.getPacket3().bytes,currentPacketController.getPacketSize());
    }
    else if (currentPacketController.getType() == TelemetryPacketType::PACKET_4){
        LOGGING(DEBUG_GS,"Sending packet 4");
        radioModule->transmitInterrupt(currentPacketController.getPacket4().bytes,currentPacketController.getPacketSize());
    }
    else{
        Console.println("Error when generating increment packet"); 
    }

    // radioModule->receiveMode();
}