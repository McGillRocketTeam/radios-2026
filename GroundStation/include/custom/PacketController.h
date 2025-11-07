#ifndef PACKET_CONTROLLER_H
#define PACKET_CONTROLLER_H

#include <cstdint>
#include <cstddef>
#include "TelemetryPacket.h"

/**
 * @enum TelemetryPacketType
 * @brief Enumerates the different telemetry packet types handled by PacketController.
 */
enum class TelemetryPacketType {
    PACKET_1,   ///< Custom telemetry packet type 1
    PACKET_2,   ///< Custom telemetry packet type 2
    PACKET_3,   ///< Custom telemetry packet type 3
    PACKET_4,   ///< Custom telemetry packet type 4
    ACK,        ///< Acknowledgment text packet
    PLAIN_TEXT  ///< Plain text message packet
};

/**
 * @class PacketController
 * @brief Manages decoding, classification, and access to different telemetry packet types.
 *
 * Uses a union to store only one packet at a time, minimizing memory use.
 * Provides setter and getter methods for each supported telemetry packet type.
 */
class PacketController {
public:
    int32_t packet_RSSI;
    int32_t packet_SNR;




    /**
     * @brief Construct a new PacketController object.
     */
    PacketController();

    /**
     * @brief Sets radio meta data to the packet controller instance
     */
    void setLocalRadioData(int32_t input_RSSI, int32_t input_SNR);

    /**
     * @brief Prints the current packet type to Serial (for debugging).
     */
    void printPacketType() const;

    /**
     * @brief Set the current packet to type PACKET_1.
     * @param pkt The PACKET_1 telemetry data.
     */
    void setPacket1(const telemetry_packet_1& pkt);

    /**
     * @brief Set the current packet to type PACKET_2.
     * @param pkt The PACKET_2 telemetry data.
     */
    void setPacket2(const telemetry_packet_2& pkt);

    /**
     * @brief Set the current packet to type PACKET_3.
     * @param pkt The PACKET_3 telemetry data.
     */
    void setPacket3(const telemetry_packet_3& pkt);

    /**
     * @brief Set the current packet to type PACKET_4.
     * @param pkt The PACKET_4 telemetry data.
     */
    void setPacket4(const telemetry_packet_4& pkt);

    /**
     * @brief Set the current packet to type ACK with a string message.
     * @param ack_text A C-string containing the acknowledgment message.
     */
    void setAckText(const char* ack_text);

    /**
     * @brief Set the current packet to type PLAIN_TEXT with a string message.
     * @param text A C-string containing the plain text message.
     */
    void setText(const char* text);

    /**
     * @brief Get the current packet type.
     * @return Enum representing the stored packet type.
     */
    TelemetryPacketType getType() const;

    /**
     * @brief Get the data stored in the controller union as TELEMETRY_PACKET_1.
     * @return Const reference to the telemetry_packet_1 data.
     */
     telemetry_packet_1& getPacket1() ;

    /**
     * @brief Get the data stored in the controller union as TELEMETRY_PACKET_2.
     * @return Const reference to the telemetry_packet_2 data.
     */
     telemetry_packet_2& getPacket2() ;

    /**
     * @brief Get the data stored in the controller union as TELEMETRY_PACKET_3.
     * @return Const reference to the telemetry_packet_3 data.
     */
     telemetry_packet_3& getPacket3() ;

    /**
     * @brief Get the data stored in the controller union as TELEMETRY_PACKET_4.
     * @return Const reference to the telemetry_packet_4 data.
     */
     telemetry_packet_4& getPacket4() ;

    /**
     * @brief Get the stored acknowledgment text.
     * @return Pointer to the null-terminated C-string.
     */
    const char* getAckText() const;

    /**
     * @brief Get the stored plain text message.
     * @return Pointer to the null-terminated C-string.
     */
    const char* getText() const;

    /**
     * @brief Get the size in bytes of the currently stored packet.
     * @return Size of the packet in bytes.
     */
    size_t getPacketSize() const;

    /**
     * @brief Check if the packet contents satisfy CTS (Clear-To-Send) requirements.
     * @return True if the packet is valid and allowed to be sent.
     */
    bool checkCTS();

    /**
     * @brief Gets the rssi contained in the controller 
     */
    int32_t getRSSI() const;

    /**
     * @brief Gets the rssi contained in the controller 
     */
    int32_t getSNR() const;
    

private:
    TelemetryPacketType type;  ///< Currently stored packet type

    /**
     * @union ControllerUnion
     * @brief Stores one of the supported telemetry packet types or string-based messages.
     *
     * Only one field is active at a time, determined by the `type` enum.
     * Can be of type telemetry_packet_1, telemetry_packet_2, telemetry_packet_3,
     * telemetry_packet_4, Acknowledgment message (null-terminated),Plain text message (null-terminated) 
     */
    union ControllerUnion {
        telemetry_packet_1 packet1;  
        telemetry_packet_2 packet2;  
        telemetry_packet_3 packet3;  
        telemetry_packet_4 packet4;  
        char ack_text[100];          
        char text[256];              
    } controller;
};

#endif // PACKET_CONTROLLER_H
