#include <Arduino.h>

#include <cassert>

#include "RangeTestGSVariant.h"
#include "Config.h"
#include "CommandParser.h"
#include "ConsoleRouter.h"
#include "MqttTopics.h"
#include "GroundStation.h"

namespace
{
    void rangeTestPrintHook(const FrameView &frame,
                            ParseError state,
                            float lastRSSI,
                            float lastSNR)
    {
        if (state != ParseError::Ok)
        {
            return;
        }

        flight_atomic_data flight{};
        const uint8_t *p = frame.atomicPtr(AT_FLIGHT_ATOMIC);

        if (!(p && frame.hasAtomic(AT_FLIGHT_ATOMIC)))
        {
            return;
        }

        memcpy(&flight, p, sizeof(flight_atomic_data));

        const float FC_RSSI = -1.0f * (static_cast<int32_t>(flight.fc_rssi_dBm) / 2.0f);
        const float FC_SNR = flight.fc_snr_dB * 0.25f;
        const float FC_LastTime = flight.gps_time_last_update_s;

        if (frame.cts() || frame.ack())
        {
            const char *kind = frame.cts() ? "CTS" : "ACK";

            char buf[192];
            snprintf(buf,
                     sizeof(buf),
                     "%u,%s,GS_T:%.3f,GS_RSSI:%.2f,GS_SNR:%.2f,FC_T:%.3f,FC_RSSI:%.2f,FC_SNR:%.2f",
                     static_cast<unsigned>(frame.header()->seq),
                     kind,
                     millis() * 0.001f,
                     lastRSSI,
                     lastSNR,
                     FC_LastTime,
                     FC_RSSI,
                     FC_SNR);

            Serial.println(buf);
        }
    }
}

void RangeTestGSVariant::setup()
{
    auto &cmd = CommandParser::getInstance();
    auto role = MqttTopic::Role::CS;

    Console.begin(role, cmd);

    auto &gs = GroundStation::getInstance();
    gs.initialise(cmd);
    gs.setCanTXFromCTS(true);

    // For the range test the GSC only outputs csv logs
    // We are overwriting the default logs
    gs.setPrintHook(rangeTestPrintHook);
    // Block printing from any other source, except crit logs
    LoggerGS::getInstance().setCategoryMask(0);
}

void RangeTestGSVariant::loop()
{
    delay(1);

    Console.handleConsoleReconnect();
    Console.mqttLoop();

    auto &gs = GroundStation::getInstance();

    gs.handleCommandParserUpdate();
    gs.handleGroundCommand();
    gs.handleReceivedPacket();
}
