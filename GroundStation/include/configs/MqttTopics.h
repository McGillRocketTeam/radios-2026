#pragma once
#include <Arduino.h>

namespace MqttTopic {

    // User-chosen
    enum class Role : uint8_t {
        CS = 0,
        PD,
        COUNT
    };

    // Programmatically decided
    enum class Band : uint8_t {
        A = 0,
        B,
        COUNT
    };

    enum class TopicKind : uint8_t {
        FC_TELEMETRY = 0,
        ACKS,
        COMMANDS,
        NAME,
        RADIO_TELEMETRY,
        STATUS, // enum: ("OK", "FAILED", "UNAVAIL", "DISABLED")
        DETAIL, // string: (equivalent to long_status prior)
        DEBUG,
        COUNT
    };

    // [role][band][kind]
    static constexpr const char* TOPIC_TABLE
        [(int)Role::COUNT][(int)Band::COUNT][(int)TopicKind::COUNT] =
    {
        // ---- Role::CS ----
        {
            // Band::A
            {
                "SystemA/Rocket/FlightComputer/telemetry",
                "SystemA/Rocket/FlightComputer/acks",
                "SystemA/Rocket/FlightComputer/commands",
                "radio-control-station-a",
                "SystemA/ControlStation/Radio/telemetry",
                "SystemA/ControlStation/Radio/status",
                "SystemA/ControlStation/Radio/detail",
                "SystemA/ControlStation/Radio/debug",
            },
            // Band::B
            {
                "SystemB/Rocket/FlightComputer/telemetry",
                "SystemB/Rocket/FlightComputer/acks",
                "SystemB/Rocket/FlightComputer/commands",
                "radio-control-station-a",
                "SystemB/ControlStation/Radio/telemetry",
                "SystemB/ControlStation/Radio/status",
                "SystemB/ControlStation/Radio/detail",
                "SystemB/ControlStation/Radio/debug",
            },
        },

        // ---- Role::PD ----
        {
            // Band::A
            {
                "SystemA/Rocket/FlightComputer/telemetry",
                "SystemA/Rocket/FlightComputer/acks",
                "SystemA/Rocket/FlightComputer/commands",
                "radio-control-station-a",
                "SystemA/Pad/Radio/telemetry",
                "SystemA/Pad/Radio/status",
                "SystemA/Pad/Radio/detail",
                "SystemA/Pad/Radio/debug",
            },
            // Band::B
            {
                "SystemA/Rocket/FlightComputer/telemetry",
                "SystemA/Rocket/FlightComputer/acks",
                "SystemA/Rocket/FlightComputer/commands",
                "radio-control-station-a",
                "SystemA/Pad/Radio/telemetry",
                "SystemA/Pad/Radio/status",
                "SystemA/Pad/Radio/detail",
                "SystemA/Pad/Radio/debug",
            },
        }
    };

    static inline const char* topic(Role role, Band band, TopicKind kind) {
        return TOPIC_TABLE[(int)role][(int)band][(int)kind];
    }

} // namespace MqttTopic
