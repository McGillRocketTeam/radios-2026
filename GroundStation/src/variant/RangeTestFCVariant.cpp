#include <Arduino.h>
#include <cstring>

#include "RangeTestFCVariant.h"

#include "Config.h"
#include "RadioModule.h"
#include "command_packet.h"
#include "frame_builder.h"
#include "frame_header.h"
#include "frame_view.h"
#include "telemetry_packets.h"

// ========================================================
// Variant of main.cpp for RANGE TEST
// that makes the radio act like a simplified mock FC

// === Finite State Machine ===
enum class FCState : uint8_t
{
    Transmit_CTS_Handshake,
    Transmit_CTS,
    Waiting_For_Response,
    Transmit_ACK
};

static bool setFlagsFromState(FCState state,
                              uint8_t &flags,
                              uint8_t &ack_id);

static void handleCommandPacket(const uint8_t *buffer, size_t packetLength, uint8_t &ack_number);

// === Globals ===
static FCState currentState = FCState::Transmit_CTS_Handshake;
static FCState previousState = FCState::Transmit_CTS_Handshake;
static uint16_t sequenceNumber = 1;
static uint8_t ack_id = 0;
static uint8_t flags = 0;

static RadioModule *radioModule = nullptr;

void RangeTestFCVariant::setup()
{
    Serial.begin(GS_SERIAL_BAUD_RATE);
    const uint32_t start = millis();
    while (!Serial && (millis() - start < 300))
    {
    }

    radioModule = new RadioModule();
}

void RangeTestFCVariant::loop()
{
    // Range tests only carry recovery and FC internal telemetry.
    uint8_t frameBuf[sizeof(FrameHeader) + sizeof(recov_atomic_data) + sizeof(fc_internal_atomic_data)] = {0};
    size_t frameLen = 0;

    if (!setFlagsFromState(currentState, flags, ack_id))
    {
        Serial.println("Invalid state of FSM cant set flags");
        return;
    }

    recov_atomic_data recovery{};
    recovery.drogue_armed_HW = true;
    recovery.drogue_energizedGate_HW = true;
    recovery.drogue_continuity_HW = true;
    recovery.main_armed_HW = true;
    recovery.main_energizedGate_HW = true;
    recovery.main_continuity_HW = true;

    fc_internal_atomic_data flight{};
    flight.fc_time_since_start = millis() / 1000;
    flight.fc_temp_celsius_em2 = 2;
    flight.battery_current_draw_mA = 3;
    flight.battery_voltage_dV = 1;
    flight.fc_rssi_dBm = (uint8_t)(radioModule->getRSSI() * -2.0f);
    flight.fc_snr_dB = (int8_t)(radioModule->getSNR() * 4.0f);
    flight.vx_sense_SW = true;

    FrameBuilder builder(frameBuf, sizeof(frameBuf));
    if (!builder.addAtomic(AT_RECOV_ATOMIC, &recovery, sizeof(recovery)) ||
        !builder.addAtomic(AT_FC_INTERNAL_ATOMIC, &flight, sizeof(flight)))
    {
        Serial.println("Error: atomic size mismatch or buffer too small.");
        return;
    }
    frameLen = builder.finalize(sequenceNumber++, flags, ack_id);

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

    // Finite State Machine Block

    if ((txSuccess && currentState == FCState::Transmit_CTS) ||
        (txSuccess && currentState == FCState::Transmit_CTS_Handshake))
    {
        previousState = currentState;
        Serial.println("We are now waiting for a response");
        currentState = FCState::Waiting_For_Response;

        radioModule->receiveMode();

        // We need to wait for the response
        uint32_t start = millis();
        while ((millis() - start) < 1000)
        {
            if (radioModule->pollValidPacketRx())
            {
                // If we get a response that is valid
                currentState = FCState::Transmit_ACK;
                delay(30);

                if (previousState == FCState::Transmit_CTS_Handshake)
                {
                    previousState = FCState::Transmit_CTS;
                }

                uint8_t *buf = radioModule->getPacketData();
                handleCommandPacket(buf, radioModule->getPacketLength(), ack_id);
                break;
            }

            delay(5);
        }
    }
    else if (txSuccess && currentState == FCState::Transmit_ACK)
    {
        currentState = FCState::Transmit_CTS;
    }

    // waiting for response means we txed a CTS and failed
    // so need to go into CTS again
    // use the previous state to see if we should go back into handshake or nah
    if (currentState == FCState::Waiting_For_Response)
    {
        Serial.println("We failed to get a response from our CTS, will send a new packet with the CTS");
        currentState = previousState;
    }

    Serial.print("RSSI:");
    Serial.print(radioModule->getRSSI());
    Serial.print("SNR:");
    Serial.println(radioModule->getSNR());
    // This delay throttles the speed, of the packet sending, the GS should always be faster
    delay(10);
}

static bool setFlagsFromState(FCState state,
                              uint8_t &flags,
                              uint8_t &ack_id)
{
    flags = 0;

    switch (state)
    {
    case FCState::Transmit_CTS_Handshake:
        flags = FLAG_CTS;
        // we are still waiting for the first response
        // reset hard the packet_id / sequence = 1
        ack_id = 0;
        sequenceNumber = 1;
        return true;

    case FCState::Transmit_CTS:
        flags = FLAG_CTS;
        ack_id = 0;
        return true;

    case FCState::Waiting_For_Response:
        // We should never be waiting for a response
        // when setting the flags, so do nothing
        return false;

    case FCState::Transmit_ACK:
        flags = FLAG_ACK;
        // ack_id must already be set by RX logic
        return true;
    }
    return true;
}


static void handleCommandPacket(const uint8_t *buffer,
                                size_t packetLength,
                                uint8_t &ack_number)
{
    // Must contain the full struct
    if (packetLength < sizeof(command_packet_data))
    {
        Serial.println("ERROR: Packet too short");
        return;
    }

    command_packet_data pkt;
    memcpy(&pkt, buffer, sizeof(pkt));

    pkt.command_string[4] = '\0';

    // For the test we dont care what the command is
    // And we will just hardcode the ack number
    ack_number = 1;
}
