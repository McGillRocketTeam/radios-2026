#pragma once

#include "CommandLine.h"
#include "command_packet.h"

#include <cstdint>
#include <Arduino.h>

// Simple util file to parse the raw input into a rocket packet
namespace RocketCommand{
    bool fillCommandPacket(const command_line& line, command_packet_extended& out);
    
    // Provide Serial as the outlet for the prints
    void printCommandPacket(const command_packet_extended& pkt, Print& out);

    bool tryParseCommandId(const command_line& line, uint8_t& outId);
}