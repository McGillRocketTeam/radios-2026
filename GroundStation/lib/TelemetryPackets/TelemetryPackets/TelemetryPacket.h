#ifndef TELEMETRYPACKET_H
#define TELEMETRYPACKET_H

#include <cstdint>
#include "BooleanDataPacket.h"
#include "FlightComputerDataPacket.h"
#include "PayloadDataPacket.h"
#include "RadioDataPacket.h"

typedef struct __attribute__((__packed__)) packet_data_1 {
    booleans_packet BooleanPacket;
    flight_computer_packet FlightComputerPacket;
    payload_packet PayloadPacket;
    radio_packet RadioPacket;
    variable_packet_1 VariablePacket;
} packet_data_1;

typedef struct __attribute__((__packed__)) packet_data_2 {
    booleans_packet BooleanPacket;
    flight_computer_packet FlightComputerPacket;
    payload_packet PayloadPacket;
    radio_packet RadioPacket;
    variable_packet_2 VariablePacket;
} packet_data_2;

typedef struct __attribute__((__packed__)) packet_data_3 {
    booleans_packet BooleanPacket;
    flight_computer_packet FlightComputerPacket;
    payload_packet PayloadPacket;
    radio_packet RadioPacket;
    variable_packet_3 VariablePacket;
} packet_data_3;

typedef struct __attribute__((__packed__)) packet_data_4 {
    booleans_packet BooleanPacket;
    flight_computer_packet FlightComputerPacket;
    payload_packet PayloadPacket;
    radio_packet RadioPacket;
    variable_packet_4 VariablePacket;
} packet_data_4;

typedef union telemetry_packet_1 {
    packet_data_1 packets;
    uint8_t bytes[sizeof(packets)];
  } telemetry_packet_1;

typedef union telemetry_packet_2 {
    packet_data_2 packets;
    uint8_t bytes[sizeof(packets)];
} telemetry_packet_2;

typedef union telemetry_packet_3 {
    packet_data_3 packets;
    uint8_t bytes[sizeof(packets)];
} telemetry_packet_3;

typedef union telemetry_packet_4 {
    packet_data_4 packets;
    uint8_t bytes[sizeof(packets)];
} telemetry_packet_4;

#endif //TELEMETRYPACKET_H
