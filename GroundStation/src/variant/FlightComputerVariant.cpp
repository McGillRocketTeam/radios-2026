#include <Arduino.h>
#include <cstring>

#include "FlightComputerVariant.h"
#include "Config.h"
#include "RadioModule.h"

#include "command_packet.h"
#include "frame_builder.h"
#include "frame_header.h"
#include "frame_printer.h"
#include "frame_view.h"
#include "telemetry_packets.h"
#include "telemetry_generator.h"

// === Finite State Machine for mock FC ===

enum class FCState : uint8_t
{
    Transmit_NoCTS,
    Transmit_NoCTS_NAK,
    Transmit_CTS,
    Waiting_For_Response,
    Transmit_ACK
};

// === Globals ===

static FCState currentState = FCState::Transmit_NoCTS;
static uint16_t sequenceNumber = 1;
static uint8_t ack_id = 0;
static uint8_t flags = 0;

static RadioModule *radioModule = nullptr;

// === Sets the flags of the next ASTRA packet based on FSM ===
static bool setFlagsFromState(FCState state,
                              uint8_t &flags,
                              uint8_t &ack_id)
{
    flags = 0;

    switch (state)
    {
    case FCState::Transmit_NoCTS:
        ack_id = 0;
        return true;

    case FCState::Transmit_NoCTS_NAK:
        flags = FLAG_NAK;
        ack_id = 0;
        return true;

    case FCState::Transmit_CTS:
        flags = FLAG_CTS;
        ack_id = 0;
        return true;

    case FCState::Waiting_For_Response:
        return false;

    case FCState::Transmit_ACK:
        flags = FLAG_ACK;
        return true;
    }
    return true;
}

// === Printing and remembering the ack from a command ===
static void handleCommandPacket(const uint8_t* buffer,
                                size_t packetLength,
                                uint8_t& ack_number)
{
    // Must contain the full struct
    if (packetLength < sizeof(command_packet_data)) {
        Serial.println("ERROR: Packet too short");
        return;
    }
    
    command_packet_data pkt;
    command_packet_extended extended;
    if (packetLength == sizeof(command_packet_extended_data)) {
        memcpy(&extended, buffer, sizeof(extended));
        char debugBuf[128];
        snprintf(debugBuf,sizeof(debugBuf),"Extended packet detected: argc: %d ,%.2f,%.2f ,%.2f",
            extended.data.argc,
            extended.data.args[0],
            extended.data.args[1],
            extended.data.args[2]
            );
        Serial.print(debugBuf);
        pkt = extended.data.base.data;
    }
    else if (packetLength == sizeof(command_packet_data)){
        memcpy(&pkt, buffer, sizeof(pkt));
    }
    else{
        Serial.println("Received command is not the right length");
    }

    pkt.command_string[5] = '\0';

    Serial.print("Command: ");
    Serial.println(pkt.command_string);

    // Case-insensitive compare for up to 4-char commands
    if (strncasecmp(pkt.command_string, "nop", 3) == 0) {
        currentState = FCState::Transmit_NoCTS_NAK;
        return;
    }
    Serial.print("Setting CMD ID: ");
    Serial.println(pkt.command_id);
    delay(1000);

    ack_number = pkt.command_id;
}




void FlightComputerVariant::setup()
{
    Serial.begin(GS_SERIAL_BAUD_RATE);
    const uint32_t start = millis();
    while (!Serial && (millis() - start < 300))
    {
    }

    Serial.println("Flight Computer Simulator");
    radioModule = new RadioModule();
}

void FlightComputerVariant::loop()
{
    uint8_t frameBuf[512] = {0};
    size_t frameLen = 0;

    if (!setFlagsFromState(currentState, flags, ack_id))
    {
        Serial.println("Invalid state of FSM cant set flags");
        return;
    }

    if (!buildTelemetryFrame(frameBuf, sizeof(frameBuf), frameLen, sequenceNumber, flags, ack_id))
    {
        Serial.println("Error: atomic size mismatch or buffer too small.");
        delay(1000);
        return;
    }

    Serial.print("Frame length: ");
    Serial.println(frameLen);
    Serial.print("Sequence: ");
    Serial.println(sequenceNumber);

    FrameView view(frameBuf, frameLen);
    auto err = view.validate();
    if (err != ParseError::Ok)
    {
        Serial.print("Validate error: ");
        Serial.println((int)err);
        delay(1000);
        return;
    }

    // Send telemetry over radio
    Serial.print("Sending telemetry over radio CTS =");
    Serial.print((flags & FLAG_CTS) ? "YES" : "NO");
    Serial.print(" ACK =");
    Serial.println((flags & FLAG_ACK) ? "YES" : "NO");

    if (!radioModule)
    {
        Serial.println("Error: Radio module not initialized!");
    }

    bool txSuccess = radioModule->transmitBlocking(frameBuf, frameLen);
    if (txSuccess)
    {
        Serial.println("Radio transmission successful!");
    }
    else
    {
        Serial.println("Radio transmission failed!");
    }

    // Finite State Machine Block

    // Success sending a CTS tx
    
    if (txSuccess && currentState == FCState::Transmit_CTS)
    {
        Serial.println("We are now waiting for a response");
        currentState = FCState::Waiting_For_Response;
        radioModule->receiveMode();

        // We need to wait for the response
        uint32_t start = millis();
        while ((millis() - start) < 1000)
        {
            if (radioModule->pollValidPacketRx())
            {
                currentState = FCState::Transmit_ACK;
                uint8_t *buf = radioModule->getPacketData();
                handleCommandPacket(buf, radioModule->getPacketLength(), ack_id);
                break;
            }
        }
    }
    else if (txSuccess && currentState == FCState::Transmit_NoCTS)
    {
        Serial.println("Successful tx of no CTS packet now swapping to with CTS");
        currentState = FCState::Transmit_CTS;
    }
    else if (txSuccess && currentState == FCState::Transmit_NoCTS_NAK)
    {
        Serial.println("Successful tx of NAK packet now swapping to with CTS");
        currentState = FCState::Transmit_CTS;
    }
    else if (txSuccess && currentState == FCState::Transmit_ACK){
        currentState = FCState::Transmit_NoCTS;
    }

    // waiting for response means we txed a CTS and failed
    // so need to go into CTS again
    if (currentState == FCState::Waiting_For_Response)
    {
        Serial.println("We failed to get a response from our CTS, will send a new packet with the CTS");
        delay(500);
        currentState = FCState::Transmit_CTS;
    }

    delay(100);
}
