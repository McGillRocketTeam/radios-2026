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
        TELEMETRY = 0,
        METADATA,
        ACKS,
        COMMANDS,
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
                "radio-controlstation-a/telemetry",
                "radio-controlstation-a/metadata",
                "radio-controlstation-a/acks",
                "radio-controlstation-a/commands",
                "radio-controlstation-a/debug"
            },
            // Band::B
            {
                "radio-controlstation-b/telemetry",
                "radio-controlstation-b/metadata",
                "radio-controlstation-b/acks",
                "radio-controlstation-b/commands",
                "radio-controlstation-b/debug"
            }
        },

        // ---- Role::PD ----
        {
            // Band::A
            {
                "radio-pad-a/telemetry",
                "radio-pad-a/metadata",
                "radio-pad-a/acks",
                "radio-pad-a/commands",
                "radio-pad-a/debug"
            },
            // Band::B
            {
                "radio-pad-b/telemetry",
                "radio-pad-b/metadata",
                "radio-pad-b/acks",
                "radio-pad-b/commands",
                "radio-pad-b/debug"
            }
        }
    };

    static inline const char* topic(Role role, Band band, TopicKind kind) {
        return TOPIC_TABLE[(int)role][(int)band][(int)kind];
    }

} // namespace MqttTopic
