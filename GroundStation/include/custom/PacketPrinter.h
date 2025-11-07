#ifndef PACKETPRINTER_H
#define PACKETPRINTER_H

#include <Arduino.h>
#include <BooleanDataPacket.h>
#include <FlightComputerDataPacket.h>
#include <PayloadDataPacket.h>
#include <TelemetryPacket.h>
#include "config.h"
#include <PacketController.h>
#include <ConsoleRouter.h>
#include <RadioParams.h>

namespace PacketPrinter
{

#define PRINT_FIELD(structure, field) \
    Console.print(#field " = ");      \
    Console.println(structure.field);

#define PRINT_TABBED_FIELD(structure, field) \
    Console.print(#field "=");               \
    Console.print(structure.field);          \
    Console.print("\t");

    void print_booleans_packet(const booleans_packet *packet);
    void print_flight_computer_packet(const flight_computer_packet *packet);
    void print_variable_packet_1(const variable_packet_1 *packet);
    void print_variable_packet_2(const variable_packet_2 *packet);
    void print_variable_packet_3(const variable_packet_3 *packet);
    void print_variable_packet_4(const variable_packet_4 *packet);
    void print_payload_packet(const payload_packet *packet);
    void print_radio_packet(const radio_packet *packet);

    template <typename T>
    void print_telemetry_packet(const T *packet);

    template <>
    void print_telemetry_packet(const telemetry_packet_1 *packet);
    template <>
    void print_telemetry_packet(const telemetry_packet_2 *packet);
    template <>
    void print_telemetry_packet(const telemetry_packet_3 *packet);
    template <>
    void print_telemetry_packet(const telemetry_packet_4 *packet);

    // This prints the packets in a special serialised format for the GUI
    // It has the header as a uint32 in the front to tell the GUI the size
    // It has the GS data SNR/RSSI just before the variable section
    template <typename T>
    void packet_to_serial_variable_length(const T *packet, PacketController *currentPacketController);

    template <>
    void packet_to_serial_variable_length(const telemetry_packet_1 *packet, PacketController *currentPacketController);
    template <>
    void packet_to_serial_variable_length(const telemetry_packet_2 *packet, PacketController *currentPacketController);
    template <>
    void packet_to_serial_variable_length(const telemetry_packet_3 *packet, PacketController *currentPacketController);
    template <>
    void packet_to_serial_variable_length(const telemetry_packet_4 *packet, PacketController *currentPacketController);

    void copy_fixed_packets(const uint8_t *radio, size_t rsz,
                            const uint8_t *boolean, size_t bsz,
                            const uint8_t *flight, size_t fsz,
                            const uint8_t *payload, size_t psz,
                            uint8_t *buffer, size_t &pos);

    void packet_to_serial_constant_length(const telemetry_packet_1 *packet, PacketController *currentPacketController);
    void packet_to_serial_constant_length(const telemetry_packet_2 *packet, PacketController *currentPacketController);
    void packet_to_serial_constant_length(const telemetry_packet_3 *packet, PacketController *currentPacketController);
    void packet_to_serial_constant_length(const telemetry_packet_4 *packet, PacketController *currentPacketController);

    void ack_to_serial(const char *ack_text, size_t ack_text_size);

    template <typename T>
    void print_telemetry_packet_id_serial(const T *packet);

    template <>
    void print_telemetry_packet_id_serial(const telemetry_packet_1 *packet);
    template <>
    void print_telemetry_packet_id_serial(const telemetry_packet_2 *packet);
    template <>
    void print_telemetry_packet_id_serial(const telemetry_packet_3 *packet);
    template <>
    void print_telemetry_packet_id_serial(const telemetry_packet_4 *packet);

    template <typename T>
    void print_packet_id(const T *packet);

    // Print only the booleans subpacket for debugging, not a sub function
    template <typename T>
    void print_only_booleans(const T *packet);

    // Print only the FC sub packet for debugging, not a sub function
    template <typename T>
    void print_only_FC_packet(const T *packet);

    // Print the radio params struct to the console in a GUI readable format
    void radio_params_to_console(const RadioParams *radioParams);

}

#endif // namespace PacketPrinter