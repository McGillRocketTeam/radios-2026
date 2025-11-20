#include <cstdint>

// Command string the char array is null terminated so we can send 2 letter commands
// e.g po\0 p1\0 p2\0
typedef struct __attribute__((__packed__)) command_packet_data {
    uint8_t command_id;
    char command_string[3];
} command_packet_data;

typedef union command_packet {
    command_packet_data data;
    uint8_t bytes[sizeof(command_packet_data)];
} command_packet;