#include "PacketController.h"
#include <Arduino.h>
#include <cstring>

// Constructor
PacketController::PacketController()
  : type(TelemetryPacketType::PLAIN_TEXT) {
    memset(controller.text, 0, sizeof(controller.text));
}

void PacketController::setLocalRadioData(int32_t input_RSSI,int32_t input_SNR){
    packet_RSSI = input_RSSI;
    packet_SNR = input_SNR;
}

void PacketController::printPacketType() const {
    switch (type) {
        case TelemetryPacketType::PACKET_1: Serial.print("Packet 1"); break;
        case TelemetryPacketType::PACKET_2: Serial.print("Packet 2"); break;
        case TelemetryPacketType::PACKET_3: Serial.print("Packet 3"); break;
        case TelemetryPacketType::PACKET_4: Serial.print("Packet 4"); break;
        case TelemetryPacketType::ACK:      Serial.print("ACK");       break;
        case TelemetryPacketType::PLAIN_TEXT: Serial.print("Plain text"); break;
        default: Serial.print("Unknown"); break;
    }
}

// Setters
void PacketController::setPacket1(const telemetry_packet_1& pkt) {
    type = TelemetryPacketType::PACKET_1;
    memcpy(&controller.packet1, &pkt, sizeof(pkt));
}
void PacketController::setPacket2(const telemetry_packet_2& pkt) {
    type = TelemetryPacketType::PACKET_2;
    memcpy(&controller.packet2, &pkt, sizeof(pkt));
}
void PacketController::setPacket3(const telemetry_packet_3& pkt) {
    type = TelemetryPacketType::PACKET_3;
    memcpy(&controller.packet3, &pkt, sizeof(pkt));
}
void PacketController::setPacket4(const telemetry_packet_4& pkt) {
    type = TelemetryPacketType::PACKET_4;
    memcpy(&controller.packet4, &pkt, sizeof(pkt));
}
void PacketController::setAckText(const char* ack_text) {
    type = TelemetryPacketType::ACK;
    strncpy(controller.ack_text, ack_text, sizeof(controller.ack_text)-1);
    controller.ack_text[sizeof(controller.ack_text)-1] = '\0';
}
void PacketController::setText(const char* text) {
    type = TelemetryPacketType::PLAIN_TEXT;
    strncpy(controller.text, text, sizeof(controller.text)-1);
    controller.text[sizeof(controller.text)-1] = '\0';
}

// Getters
TelemetryPacketType PacketController::getType() const {
    return type;
}
 telemetry_packet_1& PacketController::getPacket1()  {
    return controller.packet1;
}
 telemetry_packet_2& PacketController::getPacket2()  {
    return controller.packet2;
}
 telemetry_packet_3& PacketController::getPacket3()  {
    return controller.packet3;
}
 telemetry_packet_4& PacketController::getPacket4()  {
    return controller.packet4;
}
const char* PacketController::getAckText() const {
    return controller.ack_text;
}
const char* PacketController::getText() const {
    return controller.text;
}

size_t PacketController::getPacketSize() const {
    switch (type) {
        case TelemetryPacketType::PACKET_1: return sizeof(controller.packet1);
        case TelemetryPacketType::PACKET_2: return sizeof(controller.packet2);
        case TelemetryPacketType::PACKET_3: return sizeof(controller.packet3);
        case TelemetryPacketType::PACKET_4: return sizeof(controller.packet4);
        case TelemetryPacketType::ACK:      return strlen(controller.ack_text) + 1;
        case TelemetryPacketType::PLAIN_TEXT: return strlen(controller.text) + 1;
        default: return 0;
    }
}

bool PacketController::checkCTS() {
    switch (type) {
        case TelemetryPacketType::PACKET_1:
            return static_cast<bool>(controller.packet1.packets.RadioPacket.data.CTSFlag);
        case TelemetryPacketType::PACKET_2:
            return static_cast<bool>(controller.packet2.packets.RadioPacket.data.CTSFlag);
        case TelemetryPacketType::PACKET_3:
            return static_cast<bool>(controller.packet3.packets.RadioPacket.data.CTSFlag);
        case TelemetryPacketType::PACKET_4:
            return static_cast<bool>(controller.packet4.packets.RadioPacket.data.CTSFlag);
        default:
            return false;
    }
}


int32_t PacketController::getRSSI() const {
    return packet_RSSI;
}

int32_t PacketController::getSNR() const {
    return packet_SNR;
}
