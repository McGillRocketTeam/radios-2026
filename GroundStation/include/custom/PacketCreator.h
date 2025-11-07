#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#include <PacketController.h>

namespace PacketCreatorIncrement {
    booleans_packet generateIncrementBooleanPacket();
    flight_computer_packet generateIncrementFlightComputerPacket();
    payload_packet generateIncrementPayloadPacket();
    radio_packet generateIncrementRadioPacket();
    variable_packet_1 generateIncrementVariablePacket1();
    variable_packet_2 generateIncrementVariablePacket2();
    variable_packet_3 generateIncrementVariablePacket3();
    variable_packet_4 generateIncrementVariablePacket4();
    telemetry_packet_1 generateIncrementTelemetryPacket1();
    telemetry_packet_2 generateIncrementTelemetryPacket2();
    telemetry_packet_3 generateIncrementTelemetryPacket3();
    telemetry_packet_4 generateIncrementTelemetryPacket4();
}

namespace PacketCreatorRandom {
    booleans_packet generateRandomBooleansPacket();
    flight_computer_packet generateRandomFlightComputerPacket();
    payload_packet generateRandomPayloadPacket();
    radio_packet generateRandomRadioPacket();
    variable_packet_1 generateRandomVariablePacket1();
    variable_packet_2 generateRandomVariablePacket2();
    variable_packet_3 generateRandomVariablePacket3();
    variable_packet_4 generateRandomVariablePacket4();
    telemetry_packet_1 generateRandomTelemetryPacket1();
    telemetry_packet_2 generateRandomTelemetryPacket2();
    telemetry_packet_3 generateRandomTelemetryPacket3();
    telemetry_packet_4 generateRandomTelemetryPacket4();
}

namespace PacketCreator {
    template <typename T>
    T generateRandomPacket();

    template <typename T>
    T generateIncrementPacket();

    telemetry_packet_1 generateAllOnesTelemetryPacket();
    PacketController generateNextIncrementPacket();
}

// Template specializations must stay in header
namespace PacketCreator {
    template <>
    telemetry_packet_1 generateRandomPacket<telemetry_packet_1>();
    template <>
    telemetry_packet_2 generateRandomPacket<telemetry_packet_2>();
    template <>
    telemetry_packet_3 generateRandomPacket<telemetry_packet_3>();
    template <>
    telemetry_packet_4 generateRandomPacket<telemetry_packet_4>();

    template <>
    telemetry_packet_1 generateIncrementPacket<telemetry_packet_1>();
    template <>
    telemetry_packet_2 generateIncrementPacket<telemetry_packet_2>();
    template <>
    telemetry_packet_3 generateIncrementPacket<telemetry_packet_3>();
    template <>
    telemetry_packet_4 generateIncrementPacket<telemetry_packet_4>();
}

#endif // PACKETCREATOR_H
