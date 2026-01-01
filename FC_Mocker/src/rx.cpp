#include <RadioLib.h>
#include <cstring>
#include "config/Config.h"
#include "config/PinLayout.h"
#include "frame_header.h"
#include "frame_printer.h"

SX1262 radio = new Module(PIN_NSS, PIN_DIO1, PIN_RST, PIN_BUSY);

// Global buffer to hold incoming packets
uint8_t* rx_buffer = new uint8_t[MAX_FRAME_SIZE];

void setup() {
    // Begin serial monitoring
    Serial.begin(9600);
    delay(1500);
    Serial.println("Initialising SX1262 receiver...");

    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("SX1262 initialisation successful");
    }
    else {
        Serial.print("Init failed, code ");
        Serial.println(state);
        while (true) ;
    }

    // Configure radio
    radio.setFrequency(FREQUENCY);
    radio.setBandwidth(BANDWIDTH);
    radio.setSpreadingFactor(SPREADING_FACTOR);
}

void loop() {
    int state = radio.receive(rx_buffer, MAX_FRAME_SIZE);

    if (state == RADIOLIB_ERR_NONE) {
        // Successfully received a packet

        size_t frame_len = radio.getPacketLength();

        // Pass the raw buffer to a parsing function
        FrameView view(rx_buffer, frame_len);

        // Validate the frame
        ParseError err = view.validate();
        if (err != ParseError::Ok) {
            Serial.print("Validation Error: ");
            // Cast to int to see the ID (0=Ok, 1=TooShort, 2=PayloadTooShort, 3=UnknownAtomicSize)
            Serial.println((int)err); 
            
            // Also print the actual length received vs what SX1262 reported
            Serial.print("Bytes received: ");
            Serial.println(frame_len);

            Serial.print("RadioLib state: ");
            Serial.println(state);

            return;
        }

        for (int idx = 0; idx < AT_TOTAL; ++idx) {
            AtomicType type = static_cast<AtomicType>(idx);
            if (view.hasAtomic(type))
                printAtomic(view, type);
        }
    }
    else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
        // Timeout occurred while waiting for a packet
        Serial.println("Timeout");
    }
    else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
        // Packet was received, but it was malformed
        Serial.println("Received corrupted packet");
    }
    else {
        Serial.print("Receive failed, code ");
        Serial.println(state);
    }
}