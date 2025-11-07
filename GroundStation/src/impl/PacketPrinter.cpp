#include "custom/PacketPrinter.h"
#include <PacketController.h>


namespace PacketPrinter
{

void print_booleans_packet(const booleans_packet *packet)
{
    PRINT_FIELD(packet->data, V_EXT_SNS);
    PRINT_FIELD(packet->data, drogue_armed_SW);
    PRINT_FIELD(packet->data, drogue_armed_HW);
    PRINT_FIELD(packet->data, drogue_energized_SW);
    PRINT_FIELD(packet->data, drogue_energizedGate_HW);
    PRINT_FIELD(packet->data, drogue_energizedCurrent_HW);
    PRINT_FIELD(packet->data, drogue_continuity_HW);
    PRINT_FIELD(packet->data, main_armed_SW);
    PRINT_FIELD(packet->data, main_armed_HW);
    PRINT_FIELD(packet->data, main_energized_SW);
    PRINT_FIELD(packet->data, main_energizedGate_HW);
    PRINT_FIELD(packet->data, main_energizedCurrent_HW);
    PRINT_FIELD(packet->data, main_continuity_HW);
    PRINT_FIELD(packet->data, prop_energized_elec);
    PRINT_FIELD(packet->data, ufd_armed_SW);
    PRINT_FIELD(packet->data, ufd_armed_HW);
    PRINT_FIELD(packet->data, ufd_energized_SW);
    PRINT_FIELD(packet->data, ufd_energizedGate_HW);
    PRINT_FIELD(packet->data, ufd_energizedCurrent_HW);
    PRINT_FIELD(packet->data, ufd_continuity_HW);
    PRINT_FIELD(packet->data, run_armed_SW);
    PRINT_FIELD(packet->data, run_armed_HW);
    PRINT_FIELD(packet->data, run_energized_SW);
    PRINT_FIELD(packet->data, run_energizedGate_HW);
    PRINT_FIELD(packet->data, run_energizedCurrent_HW);
    PRINT_FIELD(packet->data, run_continuity_HW);
    PRINT_FIELD(packet->data, vent_armed_SW);
    PRINT_FIELD(packet->data, vent_armed_HW);
    PRINT_FIELD(packet->data, vent_energized_SW);
    PRINT_FIELD(packet->data, vent_energizedGate_HW);
    PRINT_FIELD(packet->data, vent_energizedCurrent_HW);
    PRINT_FIELD(packet->data, vent_continuity_HW);
    Console.println();
}

void print_flight_computer_packet(const flight_computer_packet *packet)
{
    PRINT_FIELD(packet->data, tank_pressure);
    PRINT_FIELD(packet->data, vent_temp);
    PRINT_FIELD(packet->data, flight_stage);
    PRINT_FIELD(packet->data, alt_altitude);
    PRINT_FIELD(packet->data, vertical_speed);
    PRINT_FIELD(packet->data, gps_latitude);
    PRINT_FIELD(packet->data, gps_longitude);
    PRINT_FIELD(packet->data, gps_altitude);
    PRINT_FIELD(packet->data, acceleration_x);
    PRINT_FIELD(packet->data, acceleration_y);
    PRINT_FIELD(packet->data, acceleration_z);
    PRINT_FIELD(packet->data, angle_yaw);
    PRINT_FIELD(packet->data, angle_pitch);
    PRINT_FIELD(packet->data, angle_roll);
    PRINT_FIELD(packet->data, battery_voltage);
    Console.println();
}

void print_variable_packet_1(const variable_packet_1 *packet)
{
    PRINT_FIELD(packet->data, gps_time_last_update);
    PRINT_FIELD(packet->data, tank_temp);
    Console.println();
}

void print_variable_packet_2(const variable_packet_2 *packet)
{
    PRINT_FIELD(packet->data, polling_rate);
    PRINT_FIELD(packet->data, prop_cc_pressure);
    Console.println();
}

void print_variable_packet_3(const variable_packet_3 *packet)
{
    PRINT_FIELD(packet->data, time_from_sw_reset);
    PRINT_FIELD(packet->data, tank_temp);
    PRINT_FIELD(packet->data, padding);
    Console.println();
}

void print_variable_packet_4(const variable_packet_4 *packet)
{
    PRINT_FIELD(packet->data, rx_tx_ratio);
    PRINT_FIELD(packet->data, prop_cc_pressure);
    Console.println();
}

void print_payload_packet(const payload_packet *packet)
{
    PRINT_FIELD(packet->data, pl_vibration_freq_x);
    PRINT_FIELD(packet->data, pl_vibration_freq_y);
    PRINT_FIELD(packet->data, pl_vibration_freq_z);
    PRINT_FIELD(packet->data, pl_vibration_amplitude_x);
    PRINT_FIELD(packet->data, pl_vibration_amplitude_y);
    PRINT_FIELD(packet->data, pl_vibration_amplitude_z);
    PRINT_FIELD(packet->data, pl_time_since_start);
    PRINT_FIELD(packet->data, pl_target_temp);
    PRINT_FIELD(packet->data, pl_sample_temp);
    PRINT_FIELD(packet->data, pl_battery_voltage);
    PRINT_FIELD(packet->data, pl_ok);
    PRINT_FIELD(packet->data, pl_sampling_state);
    PRINT_FIELD(packet->data, pl_active_temp_ctrl_state);
    Console.println();
}

void print_radio_packet(const radio_packet *packet)
{
    PRINT_FIELD(packet->data, CTSFlag);
    PRINT_FIELD(packet->data, packet_id);
    Console.print("HAM_call_sign = ");
    for (int i = 0; i < 6; i++)
    {
        Console.print((char)packet->data.HAM_call_sign[i]);
    }
    Console.println();
}

void print_booleans_packet_tabbed(const booleans_packet *packet){
    PRINT_TABBED_FIELD(packet->data, V_EXT_SNS);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data, drogue_armed_SW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, drogue_armed_HW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, drogue_energized_SW);
    Console.println();
    PRINT_TABBED_FIELD(packet->data, drogue_energizedGate_HW);
    PRINT_TABBED_FIELD(packet->data, drogue_energizedCurrent_HW);
    PRINT_TABBED_FIELD(packet->data, drogue_continuity_HW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, main_armed_SW);
    Console.println();
    PRINT_TABBED_FIELD(packet->data, main_armed_HW);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data, main_energized_SW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, main_energizedGate_HW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, main_energizedCurrent_HW);
    Console.println();
    PRINT_TABBED_FIELD(packet->data, main_continuity_HW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, prop_energized_elec);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, ufd_armed_SW);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data, ufd_armed_HW);
    Console.println();
    PRINT_TABBED_FIELD(packet->data, ufd_energized_SW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, ufd_energizedGate_HW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, ufd_energizedCurrent_HW);
    PRINT_TABBED_FIELD(packet->data, ufd_continuity_HW);
    Console.println();
    PRINT_TABBED_FIELD(packet->data, V_EXT_SNS);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data, run_armed_HW);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data, run_energized_SW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, run_energizedGate_HW);
    Console.println();
    PRINT_TABBED_FIELD(packet->data, run_energizedCurrent_HW);
    PRINT_TABBED_FIELD(packet->data, run_continuity_HW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, vent_armed_SW);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data, vent_armed_HW);
    Console.println();
    PRINT_TABBED_FIELD(packet->data, vent_energized_SW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, vent_energizedGate_HW);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data, vent_energizedCurrent_HW);
    PRINT_TABBED_FIELD(packet->data, vent_continuity_HW);
    Console.println();
}

void print_FC_packet_tabbed(const flight_computer_packet *packet){
    PRINT_TABBED_FIELD(packet->data,tank_pressure);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data,vent_temp);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data,flight_stage);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data,alt_altitude);
    Console.println();
    PRINT_TABBED_FIELD(packet->data,altitude_from_ground);
    Console.println();
    PRINT_TABBED_FIELD(packet->data,vertical_speed);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data,gps_latitude);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data,gps_longitude);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data,gps_altitude);
    Console.println();
    PRINT_TABBED_FIELD(packet->data,acceleration_x);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data,acceleration_y);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data,acceleration_z);
    Console.print("\t");
    PRINT_TABBED_FIELD(packet->data,angle_yaw);
    Console.println();
    PRINT_TABBED_FIELD(packet->data,angle_pitch);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data,angle_roll);
    Console.print("\t\t");
    PRINT_TABBED_FIELD(packet->data,battery_voltage);
    Console.print("\t\t");
    Console.println();
}

// Explicit template specializations for print_telemetry_packet
template <>
void print_telemetry_packet(const telemetry_packet_1 *packet)
{
    print_booleans_packet(&packet->packets.BooleanPacket);
    print_flight_computer_packet(&packet->packets.FlightComputerPacket);
    print_payload_packet(&packet->packets.PayloadPacket);
    print_radio_packet(&packet->packets.RadioPacket);
    print_variable_packet_1(&packet->packets.VariablePacket);
}
template <>
void print_telemetry_packet(const telemetry_packet_2 *packet)
{
    print_booleans_packet(&packet->packets.BooleanPacket);
    print_flight_computer_packet(&packet->packets.FlightComputerPacket);
    print_payload_packet(&packet->packets.PayloadPacket);
    print_radio_packet(&packet->packets.RadioPacket);
    print_variable_packet_2(&packet->packets.VariablePacket);
}
template <>
void print_telemetry_packet(const telemetry_packet_3 *packet)
{
    print_booleans_packet(&packet->packets.BooleanPacket);
    print_flight_computer_packet(&packet->packets.FlightComputerPacket);
    print_payload_packet(&packet->packets.PayloadPacket);
    print_radio_packet(&packet->packets.RadioPacket);
    print_variable_packet_3(&packet->packets.VariablePacket);
}
template <>
void print_telemetry_packet(const telemetry_packet_4 *packet)
{
    print_booleans_packet(&packet->packets.BooleanPacket);
    print_flight_computer_packet(&packet->packets.FlightComputerPacket);
    print_payload_packet(&packet->packets.PayloadPacket);
    print_radio_packet(&packet->packets.RadioPacket);
    print_variable_packet_4(&packet->packets.VariablePacket);
}

template <>
void packet_to_serial_variable_length(const telemetry_packet_1 *packet, PacketController *packetController)
{
    uint32_t HEADER_FOR_SIZE;
    GS_packet gs_packet{};

    constexpr size_t H  = sizeof(HEADER_FOR_SIZE);
    constexpr size_t R  = sizeof(packet->packets.RadioPacket.bytes);
    constexpr size_t B  = sizeof(packet->packets.BooleanPacket.bytes);
    constexpr size_t F  = sizeof(packet->packets.FlightComputerPacket.bytes);
    constexpr size_t P  = sizeof(packet->packets.PayloadPacket.bytes);
    constexpr size_t GS = sizeof(gs_packet.bytes);

    float gps_time_last_update = packet->packets.VariablePacket.data.gps_time_last_update;
    uint8_t tank_temp            = packet->packets.VariablePacket.data.tank_temp;

    constexpr size_t G_T = sizeof(gps_time_last_update);
    constexpr size_t T_T = sizeof(tank_temp);

    constexpr size_t TOTAL = H + R + B + F + P + GS + G_T + T_T + DELIMITER_SIZE;

    uint8_t buffer[TOTAL];
    size_t pos = 0;

    HEADER_FOR_SIZE = TOTAL - H - DELIMITER_SIZE;
    memcpy(buffer + pos, &HEADER_FOR_SIZE, H); pos += H;

    // GS data
    gs_packet.data.RSSI = packetController->packet_RSSI;
    gs_packet.data.SNR  = packetController->packet_SNR;
    memcpy(buffer + pos, gs_packet.bytes, GS); pos += GS;

    // Console.print("packet_1 header field: ");
    // Console.println(HEADER_FOR_SIZE);

    // Copy fixed packets
    copy_fixed_packets(
        packet->packets.RadioPacket.bytes, R,
        packet->packets.BooleanPacket.bytes, B,
        packet->packets.FlightComputerPacket.bytes, F,
        packet->packets.PayloadPacket.bytes, P,
        buffer, pos
    );

    // Variable fields
    memcpy(buffer + pos, &gps_time_last_update, G_T); pos += G_T;
    memcpy(buffer + pos, &tank_temp, T_T);             pos += T_T;

    // Delimiter
    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE); pos += DELIMITER_SIZE;

    Console.write(buffer, pos);
}


template <>
void packet_to_serial_variable_length(const telemetry_packet_2 *packet, PacketController *packetController)
{
    uint32_t HEADER_FOR_SIZE;
    GS_packet gs_packet{};

    constexpr size_t H  = sizeof(HEADER_FOR_SIZE);
    constexpr size_t R  = sizeof(packet->packets.RadioPacket.bytes);
    constexpr size_t B  = sizeof(packet->packets.BooleanPacket.bytes);
    constexpr size_t F  = sizeof(packet->packets.FlightComputerPacket.bytes);
    constexpr size_t P  = sizeof(packet->packets.PayloadPacket.bytes);
    constexpr size_t GS = sizeof(gs_packet.bytes);

    uint16_t polling_rate     = packet->packets.VariablePacket.data.polling_rate;
    uint16_t prop_cc_pressure = packet->packets.VariablePacket.data.prop_cc_pressure;

    constexpr size_t POLL_T = sizeof(polling_rate);
    constexpr size_t PRESS_T = sizeof(prop_cc_pressure);

    constexpr size_t TOTAL = H + R + B + F + P + GS + POLL_T + PRESS_T + DELIMITER_SIZE;

    uint8_t buffer[TOTAL];
    size_t pos = 0;

    HEADER_FOR_SIZE = TOTAL - H - DELIMITER_SIZE;
    memcpy(buffer + pos, &HEADER_FOR_SIZE, H); pos += H;

    gs_packet.data.RSSI = packetController->packet_RSSI;
    gs_packet.data.SNR  = packetController->packet_SNR;
    memcpy(buffer + pos, gs_packet.bytes, GS); pos += GS;

    // Console.print("packet_2 header field: ");
    // Console.println(HEADER_FOR_SIZE);

    copy_fixed_packets(
        packet->packets.RadioPacket.bytes, R,
        packet->packets.BooleanPacket.bytes, B,
        packet->packets.FlightComputerPacket.bytes, F,
        packet->packets.PayloadPacket.bytes, P,
        buffer, pos
    );

    

    memcpy(buffer + pos, &polling_rate, POLL_T); pos += POLL_T;
    memcpy(buffer + pos, &prop_cc_pressure, PRESS_T); pos += PRESS_T;

    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE); pos += DELIMITER_SIZE;

    Console.write(buffer, pos);
}

template <>
void packet_to_serial_variable_length(const telemetry_packet_3 *packet, PacketController *packetController)
{
    uint32_t HEADER_FOR_SIZE;
    GS_packet gs_packet{};

    constexpr size_t H  = sizeof(HEADER_FOR_SIZE);
    constexpr size_t R  = sizeof(packet->packets.RadioPacket.bytes);
    constexpr size_t B  = sizeof(packet->packets.BooleanPacket.bytes);
    constexpr size_t F  = sizeof(packet->packets.FlightComputerPacket.bytes);
    constexpr size_t P  = sizeof(packet->packets.PayloadPacket.bytes);
    constexpr size_t GS = sizeof(gs_packet.bytes);

    uint32_t time_from_sw_reset = packet->packets.VariablePacket.data.time_from_sw_reset;
    uint8_t tank_temp           = packet->packets.VariablePacket.data.tank_temp;
    uint8_t padding             = 0;

    constexpr size_t TIME_T = sizeof(time_from_sw_reset);
    constexpr size_t T_T    = sizeof(tank_temp);
    constexpr size_t PADDING_T    = sizeof(padding);


    constexpr size_t TOTAL = H + R + B + F + P + GS + TIME_T + T_T + PADDING_T + DELIMITER_SIZE;

    uint8_t buffer[TOTAL];
    size_t pos = 0;

    HEADER_FOR_SIZE = TOTAL - H - DELIMITER_SIZE;
    memcpy(buffer + pos, &HEADER_FOR_SIZE, H); pos += H;

    gs_packet.data.RSSI = packetController->packet_RSSI;
    gs_packet.data.SNR  = packetController->packet_SNR;
    memcpy(buffer + pos, gs_packet.bytes, GS); pos += GS;

    // Console.print("packet_3 header field: ");
    // Console.println(HEADER_FOR_SIZE);

    copy_fixed_packets(
        packet->packets.RadioPacket.bytes, R,
        packet->packets.BooleanPacket.bytes, B,
        packet->packets.FlightComputerPacket.bytes, F,
        packet->packets.PayloadPacket.bytes, P,
        buffer, pos
    );

    memcpy(buffer + pos, &time_from_sw_reset, TIME_T); pos += TIME_T;
    memcpy(buffer + pos, &tank_temp, T_T);             pos += T_T;
    memcpy(buffer + pos, &padding, PADDING_T);         pos += PADDING_T;

    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE); pos += DELIMITER_SIZE;

    Console.write(buffer, pos);
}

template <>
void packet_to_serial_variable_length(const telemetry_packet_4 *packet, PacketController *packetController)
{
    uint32_t HEADER_FOR_SIZE;
    GS_packet gs_packet{};

    constexpr size_t H  = sizeof(HEADER_FOR_SIZE);
    constexpr size_t R  = sizeof(packet->packets.RadioPacket.bytes);
    constexpr size_t B  = sizeof(packet->packets.BooleanPacket.bytes);
    constexpr size_t F  = sizeof(packet->packets.FlightComputerPacket.bytes);
    constexpr size_t P  = sizeof(packet->packets.PayloadPacket.bytes);
    constexpr size_t GS = sizeof(gs_packet.bytes);

    uint8_t rx_tx_ratio       = packet->packets.VariablePacket.data.rx_tx_ratio;
    uint16_t prop_cc_pressure = packet->packets.VariablePacket.data.prop_cc_pressure;

    constexpr size_t RATIO_T = sizeof(rx_tx_ratio);
    constexpr size_t PRESS_T = sizeof(prop_cc_pressure);

    constexpr size_t TOTAL = H + R + B + F + P + GS + RATIO_T + PRESS_T + DELIMITER_SIZE;

    uint8_t buffer[TOTAL];
    size_t pos = 0;
    
    HEADER_FOR_SIZE = TOTAL - H - DELIMITER_SIZE;
    memcpy(buffer + pos, &HEADER_FOR_SIZE, H); pos += H;

    gs_packet.data.RSSI = packetController->packet_RSSI;
    gs_packet.data.SNR  = packetController->packet_SNR;
    memcpy(buffer + pos, gs_packet.bytes, GS); pos += GS;

    // Console.print("packet_4 header field: ");
    // Console.println(HEADER_FOR_SIZE);

    copy_fixed_packets(
        packet->packets.RadioPacket.bytes, R,
        packet->packets.BooleanPacket.bytes, B,
        packet->packets.FlightComputerPacket.bytes, F,
        packet->packets.PayloadPacket.bytes, P,
        buffer, pos
    );

    memcpy(buffer + pos, &rx_tx_ratio, RATIO_T); pos += RATIO_T;
    memcpy(buffer + pos, &prop_cc_pressure, PRESS_T); pos += PRESS_T;

    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE); pos += DELIMITER_SIZE;

    Console.write(buffer, pos);
}


void copy_fixed_packets(const uint8_t *radio, size_t rsz,
                        const uint8_t *boolean, size_t bsz,
                        const uint8_t *flight, size_t fsz,
                        const uint8_t *payload, size_t psz,
                        uint8_t *buffer, size_t &pos)
{
    memcpy(buffer + pos, radio, rsz);
    pos += rsz;
    memcpy(buffer + pos, boolean, bsz);
    pos += bsz;
    memcpy(buffer + pos, flight, fsz);
    pos += fsz;
    memcpy(buffer + pos, payload, psz);
    pos += psz;
}

void packet_to_serial_constant_length(const telemetry_packet_1 *packet, PacketController *packetController)
{
    constexpr size_t R = sizeof(packet->packets.RadioPacket.bytes);
    constexpr size_t B = sizeof(packet->packets.BooleanPacket.bytes);
    constexpr size_t F = sizeof(packet->packets.FlightComputerPacket.bytes);
    constexpr size_t P = sizeof(packet->packets.PayloadPacket.bytes);

    all_variable_data v{};
    v.gps_time_last_update = packet->packets.VariablePacket.data.gps_time_last_update;
    v.tank_temp = packet->packets.VariablePacket.data.tank_temp;
    v.RSSI = packetController->packet_RSSI;
    v.SNR = packetController->packet_SNR;

    const size_t TOTAL = R + B + F + P + sizeof(v) + DELIMITER_SIZE;
    uint8_t buffer[TOTAL];
    size_t pos = 0;

    copy_fixed_packets(
        packet->packets.RadioPacket.bytes, R,
        packet->packets.BooleanPacket.bytes, B,
        packet->packets.FlightComputerPacket.bytes, F,
        packet->packets.PayloadPacket.bytes, P,
        buffer, pos);

    memcpy(buffer + pos, &v, sizeof(v));
    pos += sizeof(v);

    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE);
    pos += DELIMITER_SIZE;
    // Console.println(TOTAL);

    Console.write(buffer, pos);
}

void packet_to_serial_constant_length(const telemetry_packet_2 *packet, PacketController *packetController)
{
    constexpr size_t R = sizeof(packet->packets.RadioPacket.bytes);
    constexpr size_t B = sizeof(packet->packets.BooleanPacket.bytes);
    constexpr size_t F = sizeof(packet->packets.FlightComputerPacket.bytes);
    constexpr size_t P = sizeof(packet->packets.PayloadPacket.bytes);

    all_variable_data v{};
    v.polling_rate = packet->packets.VariablePacket.data.polling_rate;
    v.prop_cc_pressure = packet->packets.VariablePacket.data.prop_cc_pressure;
    v.RSSI = packetController->packet_RSSI;
    v.SNR = packetController->packet_SNR;

    const size_t TOTAL = R + B + F + P + sizeof(v) + DELIMITER_SIZE;
    uint8_t buffer[TOTAL];
    size_t pos = 0;

    copy_fixed_packets(
        packet->packets.RadioPacket.bytes, R,
        packet->packets.BooleanPacket.bytes, B,
        packet->packets.FlightComputerPacket.bytes, F,
        packet->packets.PayloadPacket.bytes, P,
        buffer, pos);

    memcpy(buffer + pos, &v, sizeof(v));
    pos += sizeof(v);

    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE);
    pos += DELIMITER_SIZE;
    // Console.println(TOTAL);

    Console.write(buffer, pos);
}

void packet_to_serial_constant_length(const telemetry_packet_3 *packet, PacketController *packetController)
{
    constexpr size_t R = sizeof(packet->packets.RadioPacket.bytes);
    constexpr size_t B = sizeof(packet->packets.BooleanPacket.bytes);
    constexpr size_t F = sizeof(packet->packets.FlightComputerPacket.bytes);
    constexpr size_t P = sizeof(packet->packets.PayloadPacket.bytes);

    all_variable_data v{};
    v.time_from_sw_reset = packet->packets.VariablePacket.data.time_from_sw_reset;
    v.tank_temp = packet->packets.VariablePacket.data.tank_temp;
    v.RSSI = packetController->packet_RSSI;
    v.SNR = packetController->packet_SNR;

    const size_t TOTAL = R + B + F + P + sizeof(v) + DELIMITER_SIZE;
    uint8_t buffer[TOTAL];
    size_t pos = 0;

    copy_fixed_packets(
        packet->packets.RadioPacket.bytes, R,
        packet->packets.BooleanPacket.bytes, B,
        packet->packets.FlightComputerPacket.bytes, F,
        packet->packets.PayloadPacket.bytes, P,
        buffer, pos);

    memcpy(buffer + pos, &v, sizeof(v));
    pos += sizeof(v);

    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE);
    pos += DELIMITER_SIZE;
    // Serial.println(TOTAL);

    Console.write(buffer, pos);
}

void packet_to_serial_constant_length(const telemetry_packet_4 *packet, PacketController *packetController)
{
    constexpr size_t R = sizeof(packet->packets.RadioPacket.bytes);
    constexpr size_t B = sizeof(packet->packets.BooleanPacket.bytes);
    constexpr size_t F = sizeof(packet->packets.FlightComputerPacket.bytes);
    constexpr size_t P = sizeof(packet->packets.PayloadPacket.bytes);

    all_variable_data v{};
    v.rx_tx_ratio = packet->packets.VariablePacket.data.rx_tx_ratio;
    v.prop_cc_pressure = packet->packets.VariablePacket.data.prop_cc_pressure;
    v.RSSI = packetController->packet_RSSI;
    v.SNR = packetController->packet_SNR;

    const size_t TOTAL = R + B + F + P + sizeof(v) + DELIMITER_SIZE;
    uint8_t buffer[TOTAL];
    size_t pos = 0;

    copy_fixed_packets(
        packet->packets.RadioPacket.bytes, R,
        packet->packets.BooleanPacket.bytes, B,
        packet->packets.FlightComputerPacket.bytes, F,
        packet->packets.PayloadPacket.bytes, P,
        buffer, pos);

    memcpy(buffer + pos, &v, sizeof(v));
    pos += sizeof(v);

    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE);
    pos += DELIMITER_SIZE;

    // Serial.println(TOTAL);

    Console.write(buffer, pos);
}

void ack_to_serial(const char* ack_text, size_t ack_text_size)
{
    uint8_t buffer[ack_text_size + DELIMITER_SIZE];
    memcpy(buffer, ack_text, ack_text_size);
    memcpy(buffer + ack_text_size, DELIMITER, DELIMITER_SIZE);
    Console.write(buffer, ack_text_size + DELIMITER_SIZE);
}

// Explicit template specializations for print_telemetry_packet_id
template <>
void print_telemetry_packet_id_serial(const telemetry_packet_1 *packet)
{
    PRINT_FIELD(packet->packets.RadioPacket.data, packet_id);
}
template <>
void print_telemetry_packet_id_serial(const telemetry_packet_2 *packet)
{
    PRINT_FIELD(packet->packets.RadioPacket.data, packet_id);
}
template <>
void print_telemetry_packet_id_serial(const telemetry_packet_3 *packet)
{
    PRINT_FIELD(packet->packets.RadioPacket.data, packet_id);
}
template <>
void print_telemetry_packet_id_serial(const telemetry_packet_4 *packet)
{
    PRINT_FIELD(packet->packets.RadioPacket.data, packet_id);
}

// Explicit template specializations for print_packet_id
template <>
void print_packet_id(const telemetry_packet_1 *packet)
{
    Console.print(packet->packets.RadioPacket.data.packet_id);
}
template <>
void print_packet_id(const telemetry_packet_2 *packet)
{
    Console.print(packet->packets.RadioPacket.data.packet_id);
}
template <>
void print_packet_id(const telemetry_packet_3 *packet)
{
    Console.print(packet->packets.RadioPacket.data.packet_id);
}
template <>
void print_packet_id(const telemetry_packet_4 *packet)
{
    Console.print(packet->packets.RadioPacket.data.packet_id);
}


template <typename T>
void print_only_booleans(const T* packet) {
    static_assert(sizeof(T) == 0, "print_only_booleans not supported for this type");
}

template <>
void print_only_booleans<telemetry_packet_1>(const telemetry_packet_1* packet) {
    print_booleans_packet_tabbed(&(packet->packets.BooleanPacket));
}

template <>
void print_only_booleans<telemetry_packet_2>(const telemetry_packet_2* packet) {
    print_booleans_packet_tabbed(&(packet->packets.BooleanPacket));
}

template <>
void print_only_booleans<telemetry_packet_3>(const telemetry_packet_3* packet) {
    print_booleans_packet_tabbed(&(packet->packets.BooleanPacket));
}

template <>
void print_only_booleans<telemetry_packet_4>(const telemetry_packet_4* packet) {
    print_booleans_packet_tabbed(&(packet->packets.BooleanPacket));
}

template <typename T>
void print_only_FC_packet(const T* packet) {
    static_assert(sizeof(T) == 0, "print_only_FC_packet not supported for this type");
}

template <>
void print_only_FC_packet<telemetry_packet_1>(const telemetry_packet_1* packet) {
    print_FC_packet_tabbed(&(packet->packets.FlightComputerPacket));
}

template <>
void print_only_FC_packet<telemetry_packet_2>(const telemetry_packet_2* packet) {
    print_FC_packet_tabbed(&(packet->packets.FlightComputerPacket));
}

template <>
void print_only_FC_packet<telemetry_packet_3>(const telemetry_packet_3* packet) {
    print_FC_packet_tabbed(&(packet->packets.FlightComputerPacket));
}

template <>
void print_only_FC_packet<telemetry_packet_4>(const telemetry_packet_4* packet) {
    print_FC_packet_tabbed(&(packet->packets.FlightComputerPacket));
}

void radio_params_to_console(const RadioParams *radioParams) {
    const size_t TOTAL = PRELIMITER_GS_RADIO_SIZE + sizeof(RadioParams) + DELIMITER_SIZE;
    uint8_t buffer[TOTAL];
    size_t pos = 0;

    // Copy delimiter first 
    memcpy(buffer + pos, PRELIMITER_GS_RADIO, PRELIMITER_GS_RADIO_SIZE);
    pos += PRELIMITER_GS_RADIO_SIZE;

    memcpy(buffer + pos, radioParams, sizeof(RadioParams));
    pos += sizeof(RadioParams);

    memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE);
    pos += DELIMITER_SIZE;

    Console.write(buffer, pos);
}

} // namespace PacketPrinter