#include <Arduino.h>
#include <cstring>

#include "Config.h"
#include "RadioModule.h"

#include "command_packet.h"
#include "frame_builder.h"
#include "frame_header.h"
#include "frame_printer.h"
#include "frame_view.h"
#include "telemetry_packets.h"

// ========================================================
// Variant of main.cpp for RANGE TEST
// that makes the GS mock FC
// The telemetry generation is hardcoded to Jan 2026

// === Telemetry contruction declerations ===
static void fillStates(states_atomic_data &states);
static void fillProp(prop_atomic_data &prop);
static void fillFlight(flight_atomic_data &flight);
static void fillRadio(radio_atomic_data &radio);

static bool buildTelemetryFrame(uint8_t *frameBuf,
                                size_t frameCap,
                                size_t &outFrameLen,
                                uint16_t &inOutSeqNumber,
                                uint8_t flags,
                                uint8_t ack_id);

// === Finite State Machine for mock FC ===

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

// === Printing and remembering the ack from a command
static void handleCommandPacket(const uint8_t *buffer, size_t packetLength, uint8_t &ack_number);

// === Globals ===

static FCState currentState = FCState::Transmit_CTS_Handshake;
static FCState previousState = FCState::Transmit_CTS_Handshake;
static uint16_t sequenceNumber = 1;
static uint8_t ack_id = 0;
static uint8_t flags = 0;

static RadioModule *radioModule = nullptr;

void setup()
{
    Serial.begin(GS_SERIAL_BAUD_RATE);
    const uint32_t start = millis();
    while (!Serial && (millis() - start < 300))
    {
    }

    radioModule = new RadioModule();
    LoggerGS::getInstance().setCategoryMask(CAT_RANGETEST);
}

void loop()
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
        return;
    }

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
    delay(10);
}

// === Telemetry Construction Impl ===

static bool buildTelemetryFrame(uint8_t *frameBuf,
                                size_t frameCap,
                                size_t &outFrameLen,
                                uint16_t &inOutSeqNumber,
                                uint8_t flags,
                                uint8_t ack_id)
{
    states_atomic_data states{};
    prop_atomic_data prop{};
    flight_atomic_data flight{};
    radio_atomic_data radio{};

    fillStates(states);
    fillProp(prop);
    fillFlight(flight);
    fillRadio(radio);

    FrameBuilder fb(frameBuf, frameCap);

    bool ok = true;
    ok &= fb.addAtomic((int)AT_STATES_ATOMIC, &states, sizeof(states));
    ok &= fb.addAtomic((int)AT_PROP_ATOMIC, &prop, sizeof(prop));
    ok &= fb.addAtomic((int)AT_FLIGHT_ATOMIC, &flight, sizeof(flight));
    ok &= fb.addAtomic((int)AT_RADIO_ATOMIC, &radio, sizeof(radio));
    if (!ok)
        return false;

    outFrameLen = fb.finalize(inOutSeqNumber++, flags, ack_id);
    return true;
}

static void fillStates(states_atomic_data &states)
{
    states = {};

    states.mov_hall_state = true;
    states.fdov_armed_SW = true;
    states.fdov_armed_HW = true;
    states.fdov_energized_SW = true;
    states.fdov_energizedGate_HW = true;
    states.fdov_energizedCurrent_HW = true;
    states.fdov_continuity_HW = true;

    states.mov_armed_SW = true;
    states.mov_armed_HW = false;
    states.mov_energized_SW = true;
    states.mov_energizedGate_HW = true;
    states.mov_energizedCurrent_HW = true;
    states.mov_continuity_HW = true;

    states.pilot_armed_SW = true;
    states.pilot_armed_HW = false;
    states.pilot_energized_SW = false;
    states.pilot_energizedGate_HW = false;
    states.pilot_energizedCurrent_HW = true;
    states.pilot_continuity_HW = true;

    states.ring_armed_SW = true;
    states.ring_armed_HW = true;
    states.ring_energized_SW = false;
    states.ring_energizedGate_HW = false;
    states.ring_energizedCurrent_HW = true;
    states.ring_continuity_HW = true;

    states.prop_energized_electric = true;

    states.vent_armed_SW = true;
    states.vent_armed_HW = true;
    states.vent_energized_SW = false;
    states.vent_energizedGate_HW = false;
    states.vent_energizedCurrent_HW = true;
    states.vent_continuity_HW = true;
}

static void fillProp(prop_atomic_data &prop)
{
    prop = {};

    prop.cc_pressure = 100;
    prop.tank_pressure = 1000;
    prop.tank_temp = 800;
    prop.vent_temp = 200;
}

static void fillFlight(flight_atomic_data &flight)
{
    flight = {};

    flight.flight_stage = 1;
    flight.altimeter_altitude = 5.0f;
    flight.altitude_from_sea_level = 1525.0f;
    flight.apogee_from_ground = 2000.0f;
    flight.atm_pressure = 1012.75f;
    flight.barometer_altitude = 5.0f;
    flight.atm_temp = 20.45f;

    flight.gps_latitude = 40.7128f;
    flight.gps_longitude = -74.0060f;
    flight.gps_altitude = 1530.0f;
    // Piggyback extra info inside of gps last time update
    // This will store the generation time of this packet
    // For the FC side to report the time
    flight.gps_time_last_update = millis() * 0.001f;

    flight.vertical_speed = 15.5f;

    flight.acceleration_x = 110;
    flight.acceleration_y = -40;
    flight.acceleration_z = 1000;

    flight.angle_yaw = 50;
    flight.angle_pitch = 12;
    flight.angle_roll = 7;

    flight.fc_rssi = (uint16_t)((radioModule->getRSSI() * 2.0f) + 400.0f);
    flight.fc_snr = (int8_t)(radioModule->getSNR() * 4.0f);
}

static void fillRadio(radio_atomic_data &radio)
{
    radio = {};
    strncpy(radio.call_sign, "VA2JWL", sizeof(radio.call_sign));
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
