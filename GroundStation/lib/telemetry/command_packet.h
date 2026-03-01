#pragma once

#include <cstdint>

// Command string supports up to 4 characters
// It must be null terminated

typedef struct __attribute__((__packed__)) command_packet_data {
    uint8_t command_id;
    char command_string[6];
} command_packet_data;

typedef union command_packet {
    command_packet_data data;
    uint8_t bytes[sizeof(command_packet_data)];
} command_packet;

typedef struct __attribute__((__packed__)) command_packet_extended_data {
    command_packet base;
    uint8_t argc; // 0..3
    float args[3]; // This float array should be 4 byte aligned
} command_packet_extended_data;

typedef union command_packet_extended {
    command_packet_extended_data data;
    uint8_t bytes[sizeof(command_packet_extended_data)];
} command_packet_extended;