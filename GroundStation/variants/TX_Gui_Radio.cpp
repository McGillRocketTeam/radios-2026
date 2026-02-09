#include <Arduino.h>

#include <cstring>

#include "ConsoleRouter.h"
#include "frame_builder.h"
#include "frame_header.h"
#include "frame_view.h"
#include "frame_printer.h"
#include "RadioModule.h"
#include "telemetry_packets.h"

// ======================================================== 
// Variant of main.cpp that just generates telemetry values
// and sends it to the GUI
// The telemetry generation is hardcoded to Dec 2025

static uint16_t sequenceNumber = 1;
static RadioModule *radioModule = nullptr;

void setup()
{
    Serial.begin(115200);
    const uint32_t start = millis();
    while (!Serial && (millis() - start < 300))
    { /* wait */
    }
    auto& cmd = CommandParser::getInstance();
    Console.begin(MqttTopic::Role::CS,cmd);
    Console.handleConsoleReconnect();

    Serial.println(F("Flight Computer Simulator"));
    radioModule = new RadioModule();
}

void loop()
{
    static uint32_t lastReconnect = 0;
    if (millis() - lastReconnect > 5000)
    {
        Console.handleConsoleReconnect();
        Console.mqttLoop();
        lastReconnect = millis();
    }

    states_atomic_data states{};
    static bool toggle = false;
    toggle = !toggle;

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

    prop_atomic_data prop{};
    static uint16_t pressureCounter = 1000;
    pressureCounter += 10;
    prop.cc_pressure = pressureCounter;
    prop.tank_pressure = pressureCounter + 1000;
    prop.tank_temp = 800 + (pressureCounter % 100);
    prop.vent_temp = 200 + (pressureCounter % 50);

    flight_atomic_data flight{};
    static float altitude = 0.0f;
    altitude += 5.0f;

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
    flight.acceleration_x = 100 + (pressureCounter % 20);
    flight.acceleration_y = -50 + (pressureCounter % 10);
    flight.acceleration_z = 980 + (pressureCounter % 30);
    flight.angle_yaw = 45 + (pressureCounter % 10);
    flight.angle_pitch = 10 + (pressureCounter % 5);
    flight.angle_roll = 5 + (pressureCounter % 5);
    flight.fc_rssi = 85;
    flight.fc_snr = 12;

    // Initialize radio_atomic_data
    radio_atomic_data radio{};
    strncpy(radio.call_sign, "VA2JWL", sizeof(radio.call_sign));

    uint8_t frameBuf[512] = {0};

    FrameBuilder fb(frameBuf, sizeof(frameBuf));

    bool ok = true;
    ok &= fb.addAtomic((int)AT_STATES_ATOMIC, &states, sizeof(states));
    ok &= fb.addAtomic((int)AT_PROP_ATOMIC, &prop, sizeof(prop));
    ok &= fb.addAtomic((int)AT_FLIGHT_ATOMIC, &flight, sizeof(flight));
    ok &= fb.addAtomic((int)AT_RADIO_ATOMIC, &radio, sizeof(radio));

    if (!ok)
    {
        Serial.println(F("Error: atomic size mismatch or buffer too small."));
        delay(1000);
        return;
    }

    const uint8_t flags = FLAG_CTS;
    const uint8_t ack_id = 0;
    size_t frameLen = fb.finalize(sequenceNumber++, flags, ack_id);

    Serial.print(F("Frame length: "));
    Serial.println(frameLen);
    Serial.print(F("Sequence: "));
    Serial.println(sequenceNumber - 1);
    Serial.print(F("Flags (CTS): 0x"));
    Serial.println(flags, HEX);

    // Validate frame
    FrameView view(frameBuf, frameLen);
    auto err = view.validate();
    if (err != ParseError::Ok)
    {
        Serial.print(F("Validate error: "));
        Serial.println((int)err);
        delay(1000);
        return;
    }

    // Send telemetry over radio (simulating FC sending to ground station)
    Serial.println(F("Sending telemetry over radio (CTS=1)..."));
    if (radioModule)
    {
        bool txSuccess = radioModule->transmitBlocking(frameBuf, frameLen);
        if (txSuccess)
        {
            Serial.println(F("Radio transmission successful!"));
        }
        else
        {
            Serial.println(F("Radio transmission failed!"));
        }
    }
    else
    {
        Serial.println(F("Error: Radio module not initialized!"));
    }

    Serial.println(F("Sending telemetry via ConsoleRouter..."));
    Console.sendTelemetry(frameBuf, frameLen);
    Serial.println(F("Telemetry sent via both radio and ConsoleRouter!"));

    delay(1000);
}
