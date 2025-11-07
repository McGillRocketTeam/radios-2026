#ifndef FLIGHTCOMPUTERPACKET_H
#define FLIGHTCOMPUTERPACKET_H

#include <cstdint>

typedef struct __attribute__((__packed__)) flight_computer_data
{
    uint16_t tank_pressure;   	// Tank pressure
    uint8_t vent_temp;             	// Vent temperature
    uint8_t flight_stage;          	// Flight stage
    float alt_altitude;            	// Altitude from altimeter
    float altitude_from_ground;     // FC altitude from ground in feet using barometric pressure
    int32_t vertical_speed;      	// Vertical speed
    float gps_latitude;        	// GPS latitude
    float gps_longitude;       	// GPS longitude
    float gps_altitude;        	// GPS altitude
    short acceleration_x;      	// Acceleration in X direction mili Gs
    short acceleration_y;      	// Acceleration in Y direction mili Gs
    short acceleration_z;      	// Acceleration in Z direction mili Gs
    short angle_yaw;           	// Yaw angle
    short angle_pitch;         	// Pitch angle
    short angle_roll;          	// Roll angle
    uint8_t battery_voltage;        // Battery voltage
} flight_computer_data;


typedef union __attribute__((__packed__)) flight_computer_packet{
    flight_computer_data data;
    uint8_t bytes[sizeof(flight_computer_data)];
}flight_computer_packet;


typedef struct __attribute__((__packed__)) variable_data_1
{
    float gps_time_last_update;
    uint8_t tank_temp;
} variable_data_1;

typedef union __attribute__((__packed__)) variable_packet_1 {
    variable_data_1 data;
    uint8_t bytes[sizeof(variable_data_1)];
} variable_packet_1;

typedef struct __attribute__((__packed__)) variable_data_2
{
    uint16_t polling_rate;
    uint16_t prop_cc_pressure;
} variable_data_2;

typedef union __attribute__((__packed__)) variable_packet_2 {
    variable_data_2 data;
    uint8_t bytes[sizeof(variable_data_2)];
} variable_packet_2;

typedef struct __attribute__((__packed__)) variable_data_3
{
    uint32_t time_from_sw_reset;
    uint8_t tank_temp;
    uint8_t padding; //Useless byte to make the four variable packets have different sizes for identification
} variable_data_3;

typedef union __attribute__((__packed__)) variable_packet_3 {
    variable_data_3 data;
    uint8_t bytes[sizeof(variable_data_3)];
} variable_packet_3;

typedef struct __attribute__((__packed__)) variable_data_4
{
    uint8_t rx_tx_ratio;
    uint16_t prop_cc_pressure;
} variable_data_4;

typedef union __attribute__((__packed__)) variable_packet_4 {
    variable_data_4 data;
    uint8_t bytes[sizeof(variable_data_4)];
} variable_packet_4;

typedef struct __attribute__((__packed__)) all_variable_data
{
    float gps_time_last_update;
    uint8_t tank_temp;
    uint16_t polling_rate;
    uint16_t prop_cc_pressure;
    uint32_t time_from_sw_reset;
    uint8_t rx_tx_ratio;
    int32_t RSSI;
    int32_t SNR;
} all_variable_data;

typedef union __attribute__((__packed__)) all_variable_packet
{
    all_variable_data data;
    uint8_t bytes[sizeof(all_variable_data)];
}all_variable_packet;

typedef struct __attribute__((__packed__)) GS_data
{
    int32_t RSSI;
    int32_t SNR;
} GS_data;

typedef union __attribute__((__packed__)) GS_packet
{
    GS_data data;
    uint8_t bytes[sizeof(GS_data)];
} GS_packet;


#endif //FLIGHTCOMPUTERPACKET_H
