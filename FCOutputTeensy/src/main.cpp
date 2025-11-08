#include <Arduino.h>
#include <RadioLib.h>
#include <TelemetryPacket.h>
#include <Config.h>
/*
 * Connections to Teensy 4.0
 * Pins:  13    14     15    18   22       24
 *        SCK   DIO1   BUSY  RST  FreqRes  3V3
 * ----------------------------------------------------------
 * ----------------------------------------------------------
 * ----------------------------------------------------------------
 * ----------- SX1262 Breakout ------------------------------------
 * ----------------------------------------------------------------
 * ----------------------------------------------------------
 * ----------------------------------------------------------
 *        RX1/TXD   TX1/RXD    NSS
 *        0         1          10
 *
 */

SX1262 radio = new Module(cs, irq, rst, gpio);
telemetry_packet_1 packet;
telemetry_packet_1 dataReceived;
volatile bool transmissionDone = false;
volatile bool receivedPacket = false;

void printTelemetryPacketBytes(const telemetry_packet_1 &packetToPrint);



void isrRX() {
    Serial.println("packet received done");
    receivedPacket = true;
}

bool tx = false;

void setup() {
// write your initialization code here
    Serial.begin(9600);
    Serial.println("Right before radio initialize");
    int state = radio.begin(FREQUENCY_USED, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
    Serial.print("state is: ");
    Serial.println(state);
    Serial.println("Right after radio initialize");
    radio.setDio1Action(isrRX);
    
    booleans_data boolData = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    booleans_packet boolPacket = { .data = boolData };

    variable_packet_1 variablePacket = {
        .data = {
            .battery_voltage = 742,
            .ejec_tank_temperature = 56,
            .rx_tx_ratio = 128
        }
    };

    flight_computer_packet FCPacket = {
        .data = {
            .ejec_tank_pressure = 1983,
            .tank_bot_pressure = 5123,
            .tank_top_pressure = 3142,
            .vent_temp = 27,
            .flight_stage = 3,
            .altitude = 10435.75,
            .vertical_speed = 2900,
            .gps_latitude = 37.7749,
            .gps_longitude = -122.4194,
            .gps_altitude = 30.5,
            .acceleration_x = 1024,
            .acceleration_y = 2048,
            .acceleration_z = 512,
            .angle_yaw = 215,
            .angle_pitch = 120,
            .angle_roll = 180,
            .mag_orientation_a = 1024,
            .mag_orientation_b = 2048,
            .mag_orientation_c = 4096
        }
    };

    payload_packet payloadPacket = {
        .data = {
            .pl_vibration_freq_x = 456,
            .pl_vibration_freq_y = 789,
            .pl_vibration_freq_z = 123,
            .pl_vibration_amplitude_x = 200,
            .pl_vibration_amplitude_y = 150,
            .pl_vibration_amplitude_z = 300,
            .pl_time_since_start = 10000,
            .pl_target_temp = 35,
            .pl_sample_temp = 28,
            .pl_muon_state = 1,
            .pl_sampling_state = 0,
            .pl_active_temp_ctrl_state = 1
        }
    };

    radio_packet radioPacket = {
        .data = {
            .CTSFlag = 'y',
            .packet_id = 25,
            .HAM_call_sign = { 'V', 'E', '2', 'V', 'U', 'B' }
        }
    };

    packet = {
        .packets = {
            .BooleanPacket = boolPacket,
            .FlightComputerPacket = FCPacket,
            .PayloadPacket = payloadPacket,
            .RadioPacket = radioPacket,
            .VariablePacket = variablePacket
        }
    };
    radio.startReceive();
    Serial.println("start receiving now...");
}
void loop() {
    delay(1000);
    Serial.println("Looping");
        if (receivedPacket) {
            size_t length = radio.getPacketLength();
            Serial.println("Received a packet...");

            uint8_t buffer[256] = {0};
            if (length > sizeof(buffer)) {  // Prevent buffer overflow
                Serial.println("Packet too large!");
                receivedPacket = false;
                return;
            }

            radio.readData(buffer, length);
            Serial.println("gets here");
            buffer[length] = '\0'; // manually null-terminate (safe if length < 256)
            const char* str = reinterpret_cast<const char*>(buffer);
            Serial.print("Received string: ");
            Serial.println(str);
            
            int rssi = radio.getRSSI();
            Serial.print("RSSI: ");
            Serial.println(rssi);
            int SNR = radio.getSNR();
            Serial.print("SNR: ");
            Serial.println(SNR);
        receivedPacket = false;  // Reset after processing
    }
}



void printTelemetryPacketBytes(const telemetry_packet_1 &packetToPrint) {
    Serial.println("Printing received packet:");
    for (size_t i = 0; i < sizeof(packetToPrint.bytes); ++i) {
        Serial.print("Byte ");
        Serial.print(i);
        Serial.print(": ");
        int byte = static_cast<int>(packetToPrint.bytes[i]);
        Serial.println(byte);
        if (byte != packet.bytes[i]) {
            Serial.print("PACKET IS INCORRECT");
        }
    }
    Serial.println("CORRECT PACKET RECEIVED");
}
