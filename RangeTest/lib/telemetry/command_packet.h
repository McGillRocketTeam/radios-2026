#pragma once

#include <cstdint>

// Command string supports up to 4 characters
// It must be null terminated

typedef struct __attribute__((__packed__)) command_packet_data {
    uint8_t command_id;
    char command_string[5];
} command_packet_data;

typedef union command_packet {
    command_packet_data data;
    uint8_t bytes[sizeof(command_packet_data)];
} command_packet;