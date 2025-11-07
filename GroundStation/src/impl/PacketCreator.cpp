#include "PacketCreator.h"
#include <cstring>
#include <Arduino.h> 



namespace PacketCreatorIncrement {
    static bool boolValues = false;

    // Function to toggle the boolean values between true and false
    booleans_packet generateIncrementBooleanPacket() {
         boolValues = !boolValues;
         booleans_packet packet = { .data = {
             boolValues, boolValues,boolValues,boolValues,boolValues,boolValues, boolValues,boolValues,boolValues,boolValues,         boolValues, boolValues,boolValues,boolValues,boolValues,boolValues, boolValues,boolValues,boolValues,boolValues,         boolValues, boolValues,boolValues,boolValues,boolValues,boolValues, boolValues,boolValues,boolValues,boolValues,         boolValues, boolValues
            }
         };
         return packet;
}

    // Declare static variables to keep track of the incremented values for flight_computer_data
    static uint16_t tank_pressure = 0;
    static uint8_t vent_temp = 0;
    static uint8_t flight_stage = 0;
    static float alt_altitude = 0.0f;
    static int32_t vertical_speed = 0;

    static bool lat_increaseing = true;
    static bool long_increasing = true;
    static float gps_latitude = 0.0f;
    static float gps_longitude = 0.0f;


    static float gps_altitude = 0.0f;
    static short acceleration_x = -300;
    static short acceleration_y = -250;
    static short acceleration_z = -200;
    static short angle_yaw = -150;
    static short angle_pitch = -100;
    static short angle_roll = -144;
    static uint8_t fc_battery_voltage = 0;

    flight_computer_packet generateIncrementFlightComputerPacket() {
        flight_computer_packet packet;
        packet.data.tank_pressure = tank_pressure;
        packet.data.vent_temp = vent_temp;
        packet.data.flight_stage = flight_stage;
        packet.data.alt_altitude = alt_altitude;
        packet.data.vertical_speed = vertical_speed;
        packet.data.gps_latitude = gps_latitude;
        packet.data.gps_longitude = gps_longitude;
        packet.data.gps_altitude = gps_altitude;
        packet.data.acceleration_x = acceleration_x;
        packet.data.acceleration_y = acceleration_y;
        packet.data.acceleration_z = acceleration_z;
        packet.data.angle_yaw = angle_yaw;
        packet.data.angle_pitch = angle_pitch;
        packet.data.angle_roll = angle_roll;
        packet.data.battery_voltage = fc_battery_voltage;
        tank_pressure++;           // Increment by 1
        vent_temp++;               // Increment by 1
        flight_stage++;            // Increment by 1
        alt_altitude += 1.0f;         // Increment by 1.0f
        vertical_speed += 1;          // Increment by 1

        //Increment between -90 and 90
        if (lat_increaseing){
            gps_latitude += 0.1f; 
        }
        else{
            gps_latitude -= 0.1f;
        }
        if (gps_latitude > 89){
            lat_increaseing = false;
        }
        if (gps_latitude < -89){
            lat_increaseing = true;
        }

        //Increment between -180 and 180 
        if (long_increasing){
            gps_longitude += 0.1f; 
        }
        else{
            gps_longitude -= 0.1f;
        }
        
        if (gps_longitude > 179){
            long_increasing = false;
        }
        if (gps_longitude < -179){
            long_increasing = true;
        }

        gps_altitude += 1.0f;         // Increment by 1.0f
        acceleration_x++;          // Increment by 1
        acceleration_y++;          // Increment by 1
        acceleration_z++;          // Increment by 1
        angle_yaw++;               // Increment by 1
        angle_pitch++;             // Increment by 1
        angle_roll++;              // Increment by 1
        fc_battery_voltage++;         // Increment by 1
        return packet;
    }

    // Static variables to keep track of the values for payload_data
    static uint16_t pl_vibration_freq_x = 0;
    static uint16_t pl_vibration_freq_y = 0;
    static uint16_t pl_vibration_freq_z = 0;
    static uint16_t pl_vibration_amplitude_x = 0;
    static uint16_t pl_vibration_amplitude_y = 0;
    static uint16_t pl_vibration_amplitude_z = 0;
    static uint32_t pl_time_since_start = 0;
    static uint8_t pl_target_temp = 0;
    static short pl_sample_temp = 0;
    static uint16_t pl_battery_voltage = 0;
    static bool pl_ok = false;
    static bool pl_sampling_state = false;
    static bool pl_active_temp_ctrl_state = false;

    // Function to increment the values of payload_data
    payload_packet generateIncrementPayloadPacket() {
        payload_packet packet;
        packet = {0};
        packet.data.pl_vibration_freq_x = pl_vibration_freq_x;
        packet.data.pl_vibration_freq_y = pl_vibration_freq_y;
        packet.data.pl_vibration_freq_z = pl_vibration_freq_z;
        packet.data.pl_vibration_amplitude_x = pl_vibration_amplitude_x;
        packet.data.pl_vibration_amplitude_y = pl_vibration_amplitude_y;
        packet.data.pl_vibration_amplitude_z = pl_vibration_amplitude_z;
        packet.data.pl_time_since_start = pl_time_since_start;
        packet.data.pl_target_temp = pl_target_temp;
        packet.data.pl_sample_temp = pl_sample_temp;
        packet.data.pl_battery_voltage = pl_battery_voltage;
        packet.data.pl_ok = pl_ok;
        packet.data.pl_sampling_state = pl_sampling_state;
        packet.data.pl_active_temp_ctrl_state = pl_active_temp_ctrl_state;
        pl_vibration_freq_x++;            // Increment by 1
        pl_vibration_freq_y++;            // Increment by 1
        pl_vibration_freq_z++;            // Increment by 1
        pl_vibration_amplitude_x++;       // Increment by 1
        pl_vibration_amplitude_y++;       // Increment by 1
        pl_vibration_amplitude_z++;       // Increment by 1
        pl_time_since_start++;            // Increment by 1
        pl_target_temp++;                  // Increment by 1
        pl_sample_temp++;                 // Increment by 1
        pl_battery_voltage ++;             // Increment by 1
        pl_ok = !pl_ok;                      // Toggle between true and false
        pl_sampling_state = !pl_sampling_state; // Toggle between true and false
        pl_active_temp_ctrl_state = !pl_active_temp_ctrl_state; // Toggle between true and false

        return packet;
    }

    // Static variables to keep track of the state between calls
    static uint8_t currentCTSFlag = 0; // Will alternate between 0 and 1
    static uint16_t currentPacketID = 0; // Increment the packet ID each time
    static uint8_t currentHAMCallSign[6] = {'V', 'A', '2', 'J', 'W', 'L'}; // Example HAM call sign, will increment

    // Function to generate a radio packet with incremented values
    radio_packet generateIncrementRadioPacket() {
        radio_packet packet;

        // Increment CTSFlag (alternates between 0 and 1)
        currentCTSFlag = (currentCTSFlag == 0) ? 1 : 0;
        packet.data.CTSFlag = currentCTSFlag;

        // Increment packet_id (wraps around if it exceeds 255)
        packet.data.packet_id = currentPacketID;
        currentPacketID = (currentPacketID + 1) % 200;  // This keeps it within the 0-255 range

        // Increment HAM_call_sign (example logic, cycling through letters 'A' to 'Z')
        for (int i = 0; i < 6; i++) {
            packet.data.HAM_call_sign[i] = currentHAMCallSign[i];
        }

        return packet;
    }
    
    // Declare variable_packet static variables, increment whenever they are used
    static float gps_time_last_update = 0.0f;
    static uint8_t tank_temp = 0;
    static uint16_t polling_rate = 0;
    static uint16_t prop_cc_pressure = 0;
    static uint32_t time_from_sw_reset = 0;
    static uint8_t rx_tx_ratio = 0;
    
    // Function to generate the random packet for variable_packet_1
    variable_packet_1 generateIncrementVariablePacket1() {
        variable_packet_1 packet;
        packet.data.gps_time_last_update = gps_time_last_update;
        packet.data.tank_temp = tank_temp;

        gps_time_last_update += 1.0f;
        tank_temp += 1;   

        return packet;
    }

    variable_packet_2 generateIncrementVariablePacket2() {
        variable_packet_2 packet;
        packet.data.polling_rate      = polling_rate++;
        packet.data.prop_cc_pressure  = prop_cc_pressure++;
        return packet;
    }

    variable_packet_3 generateIncrementVariablePacket3() {
        variable_packet_3 packet;
        packet.data.time_from_sw_reset = time_from_sw_reset++;
        packet.data.tank_temp          = tank_temp++;
        packet.data.padding            = 0;  // padding remains 0
        return packet;
    }

    variable_packet_4 generateIncrementVariablePacket4() {
        
        variable_packet_4 packet;
        packet.data.rx_tx_ratio       = rx_tx_ratio++;
        packet.data.prop_cc_pressure  = prop_cc_pressure++;
        return packet;
    }

    telemetry_packet_1 generateIncrementTelemetryPacket1() {
        telemetry_packet_1 packet;
        packet.packets.BooleanPacket   = generateIncrementBooleanPacket();
        packet.packets.FlightComputerPacket = generateIncrementFlightComputerPacket();
        packet.packets.PayloadPacket   = generateIncrementPayloadPacket();
        packet.packets.RadioPacket     = generateIncrementRadioPacket();
        packet.packets.VariablePacket  = generateIncrementVariablePacket1();
        return packet;
    }

    telemetry_packet_2 generateIncrementTelemetryPacket2() {
        telemetry_packet_2 packet;
        packet.packets.BooleanPacket   = generateIncrementBooleanPacket();
        packet.packets.FlightComputerPacket = generateIncrementFlightComputerPacket();
        packet.packets.PayloadPacket   = generateIncrementPayloadPacket();
        packet.packets.RadioPacket     = generateIncrementRadioPacket();
        packet.packets.VariablePacket  = generateIncrementVariablePacket2();
        return packet;
    }

    telemetry_packet_3 generateIncrementTelemetryPacket3() {
        telemetry_packet_3 packet;
        packet.packets.BooleanPacket   = generateIncrementBooleanPacket();
        packet.packets.FlightComputerPacket = generateIncrementFlightComputerPacket();
        packet.packets.PayloadPacket   = generateIncrementPayloadPacket();
        packet.packets.RadioPacket     = generateIncrementRadioPacket();
        packet.packets.VariablePacket  = generateIncrementVariablePacket3();
        return packet;
    }

    telemetry_packet_4 generateIncrementTelemetryPacket4() {
        telemetry_packet_4 packet;
        packet.packets.BooleanPacket   = generateIncrementBooleanPacket();
        packet.packets.FlightComputerPacket = generateIncrementFlightComputerPacket();
        packet.packets.PayloadPacket   = generateIncrementPayloadPacket();
        packet.packets.RadioPacket     = generateIncrementRadioPacket();
        packet.packets.VariablePacket  = generateIncrementVariablePacket4();
        return packet;
    }


}

namespace PacketCreatorRandom{

    #include <Arduino.h>
    #include <BooleanDataPacket.h>
    #include <FlightComputerDataPacket.h>
    #include <PayloadDataPacket.h>
    #include <TelemetryPacket.h>
#include <string.h>

    // Function to generate a random booleans_packet
    booleans_packet generateRandomBooleansPacket() {
        booleans_packet packet;
        packet.data.V_EXT_SNS = random(0, 2);
        packet.data.drogue_armed_SW = random(0, 2);
        packet.data.drogue_armed_HW = random(0, 2);
        packet.data.drogue_energized_SW = random(0, 2);
        packet.data.drogue_energizedGate_HW = random(0, 2);
        packet.data.drogue_energizedCurrent_HW = random(0, 2);
        packet.data.drogue_continuity_HW = random(0, 2);
        packet.data.main_armed_SW = random(0, 2);
        packet.data.main_armed_HW = random(0, 2);
        packet.data.main_energized_SW = random(0, 2);
        packet.data.main_energizedGate_HW = random(0, 2);
        packet.data.main_energizedCurrent_HW = random(0, 2);
        packet.data.main_continuity_HW = random(0, 2);
        packet.data.prop_energized_elec = random(0, 2);
        packet.data.ufd_armed_SW = random(0, 2);
        packet.data.ufd_armed_HW = random(0, 2);
        packet.data.ufd_energized_SW = random(0, 2);
        packet.data.ufd_energizedGate_HW = random(0, 2);
        packet.data.ufd_energizedCurrent_HW = random(0, 2);
        packet.data.ufd_continuity_HW = random(0, 2);
        packet.data.run_armed_SW = random(0, 2);
        packet.data.run_armed_HW = random(0, 2);
        packet.data.run_energized_SW = random(0, 2);
        packet.data.run_energizedGate_HW = random(0, 2);
        packet.data.run_energizedCurrent_HW = random(0, 2);
        packet.data.run_continuity_HW = random(0, 2);
        packet.data.vent_armed_SW = random(0, 2);
        packet.data.vent_armed_HW = random(0, 2);
        packet.data.vent_energized_SW = random(0, 2);
        packet.data.vent_energizedGate_HW = random(0, 2);
        packet.data.vent_energizedCurrent_HW = random(0, 2);
        packet.data.vent_continuity_HW = random(0, 2);
        return packet;
    }

    // Function to generate a random flight_computer_packet
    flight_computer_packet generateRandomFlightComputerPacket() {
        flight_computer_packet packet;
        packet.data.tank_pressure = random(0, 65535);  // 16-bit unsigned value
        packet.data.vent_temp = random(0, 255);        // 8-bit unsigned value
        packet.data.flight_stage = random(0, 255);     // 8-bit unsigned value
        packet.data.alt_altitude = random(0, 10000) / 100.0; // 10-bit floating-point value
        packet.data.vertical_speed = random(-1000, 1000); // 32-bit signed value
        packet.data.gps_latitude = random(-90 * 1000000, 90 * 1000000) / 1000000.0; // Latitude in range [-90, 90]
        packet.data.gps_longitude = random(-180 * 1000000, 180 * 1000000) / 1000000.0; // Longitude in range [-180, 180]
        packet.data.gps_altitude = random(0, 50000) / 10.0;  // Altitude in meters
        packet.data.acceleration_x = random(-32767, 32767); // short value
        packet.data.acceleration_y = random(-32767, 32767); // short value
        packet.data.acceleration_z = random(-32767, 32767); // short value
        packet.data.angle_yaw = random(-32767, 32767);   // 16-bit unsigned value (angle)
        packet.data.angle_pitch = random(-32767, 32767); // 16-bit unsigned value (angle)
        packet.data.angle_roll = random(-32767, 32767); // 16-bit unsigned value (angle)
        packet.data.battery_voltage = random(0, 255); // 8-bit unsigned value
        return packet;
    }

    // Function to generate a random payload_packet
    payload_packet generateRandomPayloadPacket() {
        payload_packet packet;
        packet.data.pl_vibration_freq_x = random(0, 65535);
        packet.data.pl_vibration_freq_y = random(0, 65535);
        packet.data.pl_vibration_freq_z = random(0, 65535);
        packet.data.pl_vibration_amplitude_x = random(0, 65535);
        packet.data.pl_vibration_amplitude_y = random(0, 65535);
        packet.data.pl_vibration_amplitude_z = random(0, 65535);
        packet.data.pl_time_since_start = random(0, 4294967295);
        packet.data.pl_target_temp = random(0, 255);
        packet.data.pl_sample_temp = random(-32767, 32767); //Short 
        packet.data.pl_battery_voltage = random(0, 65535);
        packet.data.pl_ok = random(0, 2); // Boolean field
        packet.data.pl_sampling_state = random(0, 2); // Boolean field
        packet.data.pl_active_temp_ctrl_state = random(0, 2); // Boolean field
        return packet;
    }

    static uint8_t currentHAMCallSign[6] = {'V', 'E', '2', 'V', 'U', 'B'}; // Example HAM call sign, will increment
    // Function to generate a random radio_packet
    radio_packet generateRandomRadioPacket() {
        radio_packet packet;
        packet.data.CTSFlag = random(0, 2);
        packet.data.packet_id = random(0, 255);
        for (int i = 0; i < 6; ++i) {
            packet.data.HAM_call_sign[i] = currentHAMCallSign[i];
        }
        return packet;
    }

    // Function to generate a random variable_packet_1
    variable_packet_1 generateRandomVariablePacket1() {
        variable_packet_1 packet;
        packet.data.gps_time_last_update = random(0, 10000) / 100.0; // Random float, GPS time since last update (in seconds)
        packet.data.tank_temp = random(0, 255); // Random uint8_t value for tank temperature
        return packet;
    }

    // Function to generate a random variable_packet_2
    variable_packet_2 generateRandomVariablePacket2() {
        variable_packet_2 packet;
        packet.data.polling_rate = random(1, 1000); // Random uint16_t value for polling rate
        packet.data.prop_cc_pressure = random(100, 1000); // Random uint16_t value for propellant control system pressure
        return packet;
    }

    // Function to generate a random variable_packet_3
    variable_packet_3 generateRandomVariablePacket3() {
        variable_packet_3 packet;
        packet.data.time_from_sw_reset = random(0, 10000); // Random uint32_t value for time since software reset (in seconds)
        packet.data.tank_temp = random(0, 255); // Random uint8_t value for tank temperature
        packet.data.padding = 0; // Padding is just a dummy value to ensure the packet structure is correct
        return packet;
    }

    // Function to generate a random variable_packet_4
    variable_packet_4 generateRandomVariablePacket4() {
        variable_packet_4 packet;
        packet.data.rx_tx_ratio = random(0, 100); // Random uint8_t value for RX/TX ratio
        packet.data.prop_cc_pressure = random(100, 1000); // Random uint16_t value for propellant control system pressure
        return packet;
    }

    // Function to generate a random telemetry packet 1
    telemetry_packet_1 generateRandomTelemetryPacket1() {
        telemetry_packet_1 packet;
        packet.packets.BooleanPacket = generateRandomBooleansPacket();
        packet.packets.FlightComputerPacket = generateRandomFlightComputerPacket();
        packet.packets.PayloadPacket = generateRandomPayloadPacket();
        packet.packets.RadioPacket = generateRandomRadioPacket();
        packet.packets.VariablePacket = generateRandomVariablePacket1();
        return packet;
    }

    // Function to generate a random telemetry packet 1
    telemetry_packet_2 generateRandomTelemetryPacket2() {
        telemetry_packet_2 packet;
        packet.packets.BooleanPacket = generateRandomBooleansPacket();
        packet.packets.FlightComputerPacket = generateRandomFlightComputerPacket();
        packet.packets.PayloadPacket = generateRandomPayloadPacket();
        packet.packets.RadioPacket = generateRandomRadioPacket();
	    packet.packets.VariablePacket = generateRandomVariablePacket2();
        return packet;
    }

    // Function to generate a random telemetry packet 1
    telemetry_packet_3 generateRandomTelemetryPacket3() {
        telemetry_packet_3 packet;
        packet.packets.BooleanPacket = generateRandomBooleansPacket();
        packet.packets.FlightComputerPacket = generateRandomFlightComputerPacket();
        packet.packets.PayloadPacket = generateRandomPayloadPacket();
        packet.packets.RadioPacket = generateRandomRadioPacket();
	    packet.packets.VariablePacket = generateRandomVariablePacket3();
        return packet;
    }

    // Function to generate a random telemetry packet 1
    telemetry_packet_4 generateRandomTelemetryPacket4() {
        telemetry_packet_4 packet;
        packet.packets.BooleanPacket = generateRandomBooleansPacket();
        packet.packets.FlightComputerPacket = generateRandomFlightComputerPacket();
        packet.packets.PayloadPacket = generateRandomPayloadPacket();
        packet.packets.RadioPacket = generateRandomRadioPacket();
        packet.packets.VariablePacket = generateRandomVariablePacket4();
        return packet;
    }

    
}

namespace PacketCreator {
    template <typename T>
    T generateRandomPacket();

    template <>
    telemetry_packet_1 generateRandomPacket<telemetry_packet_1>() {
        return PacketCreatorRandom::generateRandomTelemetryPacket1();
    }

    template <>
    telemetry_packet_2 generateRandomPacket<telemetry_packet_2>() {
        return PacketCreatorRandom::generateRandomTelemetryPacket2();
    }

    template <>
    telemetry_packet_3 generateRandomPacket<telemetry_packet_3>() {
        return PacketCreatorRandom::generateRandomTelemetryPacket3();
    }

    template <>
    telemetry_packet_4 generateRandomPacket<telemetry_packet_4>() {
        return PacketCreatorRandom::generateRandomTelemetryPacket4();
    }

    template <typename T>
    T generateIncrementPacket();

    template <>
    telemetry_packet_1 generateIncrementPacket<telemetry_packet_1>() {
        return PacketCreatorIncrement::generateIncrementTelemetryPacket1();
    }
    
    template <>
    telemetry_packet_2 generateIncrementPacket<telemetry_packet_2>() {
        return PacketCreatorIncrement::generateIncrementTelemetryPacket2();
    }
    
    template <>
    telemetry_packet_3 generateIncrementPacket<telemetry_packet_3>() {
        return PacketCreatorIncrement::generateIncrementTelemetryPacket3();
    }
    
    template <>
    telemetry_packet_4 generateIncrementPacket<telemetry_packet_4>() {
        return PacketCreatorIncrement::generateIncrementTelemetryPacket4();
    }

    telemetry_packet_1 generateAllOnesTelemetryPacket() {
        telemetry_packet_1 packet{};
    
        // --- 1) Boolean Packet: set all 32 bits to 1 via the 4‑byte array ---
        for (int i = 0; i < 4; ++i) {
            packet.packets.BooleanPacket.bytes[i] = 0xFF;
        }
    
        // --- 2) Flight Computer Packet ---
        auto &fc = packet.packets.FlightComputerPacket.data;
        fc.tank_pressure       = 1;
        fc.vent_temp           = 1;
        fc.flight_stage        = 1;
        fc.alt_altitude        = 1.0f;
        fc.vertical_speed      = 1;
        fc.gps_latitude        = 1.0f;
        fc.gps_longitude       = 1.0f;
        fc.gps_altitude        = 1.0f;
        fc.acceleration_x      = 1;
        fc.acceleration_y      = 1;
        fc.acceleration_z      = 1;
        fc.angle_yaw           = 1;
        fc.angle_pitch         = 1;
        fc.angle_roll          = 1;
        fc.battery_voltage     = 1;
    
        // --- 3) Payload Packet ---
        auto &pl = packet.packets.PayloadPacket.data;
        pl.pl_vibration_freq_x       = 1;
        pl.pl_vibration_freq_y       = 1;
        pl.pl_vibration_freq_z       = 1;
        pl.pl_vibration_amplitude_x  = 1;
        pl.pl_vibration_amplitude_y  = 1;
        pl.pl_vibration_amplitude_z  = 1;
        pl.pl_time_since_start       = 1;
        pl.pl_target_temp            = 1;
        pl.pl_sample_temp            = 1;
        pl.pl_battery_voltage        = 1;
        pl.pl_ok                     = true;
        pl.pl_sampling_state         = true;
        pl.pl_active_temp_ctrl_state = true;
    
        // --- 4) Radio Packet (leave HAM_call_sign as-is) ---
        auto &radio = packet.packets.RadioPacket.data;
        memcpy(radio.HAM_call_sign, "VE2VUB", 6);
        radio.CTSFlag        = 1;
        radio.packet_id      = 1;
        // leave radio.HAM_call_sign untouched, or if you want to reset it:
        // memcpy(radio.HAM_call_sign, "VE2VUB", 6);
    
        // --- 5) Variable Packet (variant 1 only has these two fields) ---
        auto &var = packet.packets.VariablePacket.data;
        var.gps_time_last_update = 1.0f;
        var.tank_temp            = 1;
    
        return packet;
    }
    
    PacketController generateNextIncrementPacket() {
        static int counter = 0;
        PacketController controller;
        switch (counter % 4) {
            case 0: {
                auto pkt = PacketCreatorIncrement::generateIncrementTelemetryPacket1();
                controller.setPacket1(pkt);
                break;
            }
            case 1: {
                auto pkt = PacketCreatorIncrement::generateIncrementTelemetryPacket2();
                controller.setPacket2(pkt);
                break;
            }
            case 2: {
                auto pkt = PacketCreatorIncrement::generateIncrementTelemetryPacket3();
                controller.setPacket3(pkt);
                break;
            }
            default: {
                auto pkt = PacketCreatorIncrement::generateIncrementTelemetryPacket4();
                controller.setPacket4(pkt);
                break;
            }
        }
        ++counter;
        return controller;
    }
    
}