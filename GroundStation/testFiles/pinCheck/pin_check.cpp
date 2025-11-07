#include <SPI.h>

// Define SPI pins for Teensy 4.0 (adjust if needed)
#define CS_PIN    10   // Chip select (NSS)
#define MOSI_PIN  11
#define MISO_PIN  12
#define SCK_PIN   13

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

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("SPI CS Probe Test");

    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);  // Deselect

    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

    // Example: Read register 0x42 (Version register on many Semtech chips)
    byte reg = 0x42;
    byte cmd = reg & 0x7F;  // Read command (MSB=0 for read)

    digitalWrite(CS_PIN, LOW);       // Start SPI transaction
    delayMicroseconds(5);

    SPI.transfer(cmd);               // Send address to read
    byte result = SPI.transfer(0x00); // Dummy byte to read response

    digitalWrite(CS_PIN, HIGH);      // End SPI transaction

    Serial.print("Read from 0x42: 0x");
    Serial.println(result, HEX);
}

void loop() {
    // Do nothing
}
