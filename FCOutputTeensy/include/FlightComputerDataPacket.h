#ifndef FLIGHTCOMPUTERPACKET_H
#define FLIGHTCOMPUTERPACKET_H

#include <cstdint>

typedef struct __attribute__((__packed__)) flight_computer_data
{
    uint16_t ejec_tank_pressure;  	// Ejection tank pressure
    uint16_t tank_bot_pressure;   	// Tank bottom pressure
    uint16_t tank_top_pressure;   	// Tank top pressure
    uint8_t vent_temp;             	// Vent temperature
    uint8_t flight_stage;          	// Flight stage
    float altitude;            	// Altitude from altimeter
    uint32_t vertical_speed;      	// Vertical speed
    float gps_latitude;        	// GPS latitude
    float gps_longitude;       	// GPS longitude
    float gps_altitude;        	// GPS altitude
    uint32_t acceleration_x;      	// Acceleration in X direction
    uint32_t acceleration_y;      	// Acceleration in Y direction
    uint32_t acceleration_z;      	// Acceleration in Z direction
    uint32_t angle_yaw;           	// Yaw angle
    uint32_t angle_pitch;         	// Pitch angle
    uint32_t angle_roll;          	// Roll angle
    uint32_t mag_orientation_a;   	// Magnetometer orientation A
    uint32_t mag_orientation_b;   	// Magnetometer orientation B
    uint32_t mag_orientation_c;   	// Magnetometer orientation C
} flight_computer_data;


typedef union __attribute__((__packed__)) flight_computer_packet{
    flight_computer_data data;
    uint8_t bytes[sizeof(flight_computer_data)];
}flight_computer_packet;


typedef struct __attribute__((__packed__)) variable_data_1
{
    uint16_t battery_voltage;
    uint8_t ejec_tank_temperature;
    uint8_t rx_tx_ratio;
} variable_data_1;

typedef union __attribute__((__packed__)) variable_packet_1 {
    variable_data_1 data;
    uint8_t bytes[sizeof(variable_data_1)];
} variable_packet_1;

typedef struct __attribute__((__packed__)) variable_data_2
{
    uint16_t prop_cc_pressure;
    uint8_t tank_bot_temp;
    uint16_t polling_rate;
} variable_data_2;

typedef union __attribute__((__packed__)) variable_packet_2 {
    variable_data_2 data;
    uint8_t bytes[sizeof(variable_data_2)];
} variable_packet_2;

typedef struct __attribute__((__packed__)) variable_data_3
{
    uint16_t battery_voltage;
    uint8_t ejec_tank_temperature;
    uint32_t time_from_sw_reset;
} variable_data_3;

typedef union __attribute__((__packed__)) variable_packet_3 {
    variable_data_3 data;
    uint8_t bytes[sizeof(variable_data_3)];
} variable_packet_3;

typedef struct __attribute__((__packed__)) variable_data_4
{
    uint16_t prop_cc_pressure;
    uint8_t tank_bot_temp;
    uint32_t gps_time_last_update;
    uint8_t padding; //Useless bit to make the four variable packets have different sizes for identification
} variable_data_4;

typedef union __attribute__((__packed__)) variable_packet_4 {
    variable_data_4 data;
    uint8_t bytes[sizeof(variable_data_4)];
} variable_packet_4;


#endif //FLIGHTCOMPUTERPACKET_H
