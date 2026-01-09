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
    Transmit_NoCTS,
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

static FCState currentState = FCState::Transmit_NoCTS;
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

    Serial.println("Flight Computer Simulator");
    radioModule = new RadioModule();
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

    bool txSuccess = radioModule->transmitInterrupt(frameBuf, frameLen);
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
            if (radioModule->checkInterruptReceived())
            {
                currentState = FCState::Transmit_ACK;
                uint8_t *buf = radioModule->readPacket();
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
    else if (txSuccess && currentState == FCState::Transmit_ACK){
        currentState = FCState::Transmit_NoCTS;
    }

    // waiting for response means we txed a CTS and failed
    // so need to go into CTS again
    if (currentState == FCState::Waiting_For_Response)
    {
        Serial.println("We failed to get a response from our CTS, will send a new packet with the CTS");
        delay(100);
        currentState = FCState::Transmit_CTS;
    }

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
    static bool toggle = false;
    toggle = !toggle;

    states = {};

    states.mov_hall_state = toggle;
    states.fdov_armed_SW = true;
    states.fdov_armed_HW = true;
    states.fdov_energized_SW = toggle;
    states.fdov_energizedGate_HW = toggle;
    states.fdov_energizedCurrent_HW = true;
    states.fdov_continuity_HW = true;
    states.mov_armed_SW = true;
    states.mov_armed_HW = false;
    states.mov_energized_SW = toggle;
    states.mov_energizedGate_HW = toggle;
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
    states.prop_energized_electric = toggle;
    states.vent_armed_SW = true;
    states.vent_armed_HW = true;
    states.vent_energized_SW = false;
    states.vent_energizedGate_HW = false;
    states.vent_energizedCurrent_HW = true;
    states.vent_continuity_HW = true;
}

static void fillProp(prop_atomic_data &prop)
{
    static uint16_t pressureCounter = 1000;
    pressureCounter += 10;

    prop = {};
    prop.cc_pressure = pressureCounter;
    prop.tank_pressure = pressureCounter + 1000;
    prop.tank_temp = 800 + (pressureCounter % 100);
    prop.vent_temp = 200 + (pressureCounter % 50);
}

static void fillFlight(flight_atomic_data &flight)
{
    static float altitude = 0.0f;
    altitude += 5.0f;

    // NOTE: you were using pressureCounter-derived values in flight; we keep that “shape”
    // without coupling the modules. If you want the exact same coupling, pass pressureCounter in.
    static uint16_t pseudoCounter = 1000;
    pseudoCounter += 10;

    flight = {};
    flight.flight_stage = (altitude < 100) ? 1 : ((altitude < 500) ? 2 : 3);
    flight.altimeter_altitude = altitude;
    flight.altitude_from_sea_level = altitude + 1520.0f;
    flight.apogee_from_ground = 2000.0f;
    flight.atm_pressure = 1013.25f - (altitude / 10.0f);
    flight.barometer_altitude = altitude;
    flight.atm_temp = 20.5f - (altitude / 100.0f);
    flight.gps_latitude = 40.7128f;
    flight.gps_longitude = -74.0060f;
    flight.gps_altitude = altitude + 1525.0f;
    flight.gps_time_last_update = millis() / 1000.0f;
    flight.vertical_speed = 15.5f;

    flight.acceleration_x = 100 + (pseudoCounter % 20);
    flight.acceleration_y = -50 + (pseudoCounter % 10);
    flight.acceleration_z = 980 + (pseudoCounter % 30);
    flight.angle_yaw = 45 + (pseudoCounter % 10);
    flight.angle_pitch = 10 + (pseudoCounter % 5);
    flight.angle_roll = 5 + (pseudoCounter % 5);

    flight.fc_rssi = 85;
    flight.fc_snr = 12;
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
    case FCState::Transmit_NoCTS:
        // No CTS, no ACK
        ack_id = 0;
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
    memcpy(&pkt, buffer, sizeof(pkt));

    pkt.command_string[4] = '\0';

    Serial.print("Command: ");
    Serial.println(pkt.command_string);

    // Case-insensitive compare for up to 4-char commands
    // "nop" is 3 chars; this matches "nop" regardless of case
    if (strncasecmp(pkt.command_string, "nop", 3) == 0) {
        currentState = FCState::Transmit_NoCTS;
        return;
    }
    Serial.print("Setting CMD ID: ");
    Serial.println(pkt.command_id);

    ack_number = pkt.command_id;
}
