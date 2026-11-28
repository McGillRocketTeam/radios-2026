#include "MockFlightComputer.h"

#include <cstring>
#include <strings.h>
#include "RadioReceiver.h"
#include "command_packet.h"
#include "frame_view.h"
#include "telemetry_generator.h"

void MockFlightComputer::reset()
{
    *this = MockFlightComputer{};
}

bool MockFlightComputer::poll(RadioReceiver& radio)
{
    if (!radio.isReady())
    {
        return false;
    }

    const uint32_t now = millis();
    if (state_ == State::Waiting)
    {
        if (now - stateStartedMs_ >= 1000)
        {
            state_ = State::CTS;
            status_ = "CTS timed out";
            stateStartedMs_ = now;
            waitMs_ = 600;
            return true;
        }

        const uint8_t* data = nullptr;
        size_t length = 0;
        if (!radio.receivePacket(data, length))
        {
            return false;
        }

        command_packet_data command{};
        if (length == sizeof(command_packet_extended_data))
        {
            command_packet_extended_data extended{};
            std::memcpy(&extended, data, sizeof(extended));
            command = extended.base.data;
            Serial.print("MockFC extended command argc: ");
            Serial.println(extended.argc);
        }
        else if (length == sizeof(command))
        {
            std::memcpy(&command, data, sizeof(command));
        }
        else
        {
            Serial.println("MockFC ignored command with invalid length");
            return false;
        }

        command.command_string[5] = '\0';
        Serial.print("MockFC command: ");
        Serial.println(command.command_string);
        const bool nop = strncasecmp(command.command_string, "nop", 3) == 0;
        state_ = nop ? State::NoCTS_NAK : State::ACK;
        ackId_ = nop ? 0 : command.command_id;
        status_ = nop ? "NOP received" : "Command received";
        stateStartedMs_ = now;
        waitMs_ = nop ? 100 : 1100;
        return true;
    }

    if (now - stateStartedMs_ < waitMs_)
    {
        return false;
    }

    uint8_t flags = 0;
    if (state_ == State::CTS) flags = FLAG_CTS;
    else if (state_ == State::ACK) flags = FLAG_ACK;
    else if (state_ == State::NoCTS_NAK) flags = FLAG_NAK;
    if (state_ != State::ACK) ackId_ = 0;

    uint8_t frame[512]{};
    size_t length = 0;
    const uint16_t sequence = sequence_;
    bool valid = buildTelemetryFrame(frame, sizeof(frame), length, sequence_, flags, ackId_);
    FrameView view(frame, length);
    valid = valid && view.validate() == ParseError::Ok;
    if (valid)
    {
        // Preserve the moving GPS values used by FlightComputerVariant.
        if (const auto* bytes = view.atomicPtr(AT_GPS_ATOMIC))
        {
            gps_atomic_data gps{};
            std::memcpy(&gps, bytes, sizeof(gps));
            const float phase = (now % 20000UL) / 10000.0f;
            const float offset = (phase < 1.0f ? phase : 2.0f - phase) * 0.02f - 0.01f;
            gps.gps_latitude_deg_e7 = (100.0f + offset) * 10000000.0f;
            gps.gps_longitude_deg_e7 = (-100.0f - offset) * 10000000.0f;
            gps.gps_time_last_update_s = static_cast<float>(now) / 1000.0f;
            std::memcpy(frame + (bytes - frame), &gps, sizeof(gps));
        }
    }

    const bool sent = valid && radio.transmit(frame, length);
    status_ = !valid ? "Frame build failed" : sent ? "Telemetry sent" : "Radio TX failed";
    if (sent)
    {
        lastSequence_ = sequence;
        lastFlags_ = flags;
        Serial.print("MockFC TX sequence: ");
        Serial.println(sequence);
        if (state_ == State::CTS)
        {
            state_ = State::Waiting;
            status_ = "Waiting for command";
        }
        else if (state_ == State::ACK)
        {
            state_ = State::NoCTS;
        }
        else
        {
            state_ = State::CTS;
        }
    }
    stateStartedMs_ = millis();
    waitMs_ = valid ? 100 : 1000;
    return true;
}
