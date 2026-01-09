#include "frame_builder.h"
#include "telemetry_packets.h"
#include <RadioLib.h>
#include "config/Config.h"
#include "config/PinLayout.h"

/*              Connections to Teensy 4.0
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
 */
SX1262 radio = new Module(PIN_NSS, PIN_DIO1, PIN_RST, PIN_BUSY);

// Global buffer to hold the final packet
uint8_t* tx_buffer = new uint8_t[MAX_FRAME_SIZE];

void setup() {
    // Begin serial monitoring
    Serial.begin(9600);
    delay(1500);
    Serial.println("Initialising SX1262 transmitter...");

    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("SX1262 initialisation successful");
    }
    else {
        Serial.print("Init failed, code ");
        Serial.println(state);
        while (true);
    }

    // Configure radio
    radio.setFrequency(FREQUENCY);
    radio.setBandwidth(BANDWIDTH);
    radio.setSpreadingFactor(SPREADING_FACTOR);
    radio.setOutputPower(POWER_OUTPUT);
}

void loop() {
    // Create prop atomic with dummy values
    prop_atomic_data current_prop_data;
    current_prop_data.cc_pressure = 40;
    current_prop_data.tank_pressure = 16;
    current_prop_data.tank_temp = 8;
    current_prop_data.vent_temp = 22;

    // Initialise the frame builder
    FrameBuilder builder(tx_buffer, MAX_FRAME_SIZE);

    builder.addAtomic(AT_PROP_ATOMIC, &current_prop_data, AT_SIZE[AT_PROP_ATOMIC]);

    // Finalise the frame
    static uint16_t sequence_counter = 0;
    size_t frame_len = builder.finalize(
        sequence_counter ++,
        0x01,
        0
    );

    // Transmit the buffer
    int state = radio.transmit(tx_buffer, frame_len);

    Serial.print("Sending packet #");
    Serial.print(sequence_counter);
    Serial.print(" (Size: ");
    Serial.print(frame_len);
    Serial.println(" bytes)");

    /* Testing
    uint8_t test_payload[15];
    memset(test_payload, 0xAB, 15); // Fill with dummy hex AB
    int state = radio.transmit(test_payload, 15);
    */

    // Catch errors
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("Transmit failed, code ");
        Serial.println(state);
    }

    delay(2000);
}