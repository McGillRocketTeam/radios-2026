#ifndef CONFIG_H
#define CONFIG_H

// Transmission boolean. Set value to false for receive only. Set to true for transmit.
#define transmitState false

// Both frequency values are in MHz for RadioLib
#define FREQUENCY_433 433
#define FREQUENCY_903 903

// Frequency used
#define FREQUENCY_USED FREQUENCY_903

// Refer to LoRa Datasheet for allowed bandwidths, in kHz for RadioLib
#define BANDWIDTH_USED 250

// Spreading factor (don't use SF5 or SF6 unless good reason, range from 7-12)
#define SPREADING_FACTOR_USED 8

// LoRa Coding Rate Denominator
#define CODING_RATE_USED 8

// DO NOT CHANGE THE SYNC WORD IF YOU DON'T KNOW WHAT IT DOES
#define SYNC_WORD 0x12

// Output power, in dBm
#define POWER_OUTPUT 17

// Length of the LoRa preamble
#define PREAMBLE_LENGTH 8

// Below values shouldn't change unless board specifications change
#define TCXO_VOLTAGE 3.0
#define USE_ONLY_LDO 0
#define RADIO_BUFFER_SIZE 256;

// Pin setup listed below:
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

#define cs 10 // This pin is the Chip Select (CS) pin. Also goes by NSS
#define irq 14 // This is the interrupt pin that is used to signal interrupts. Normally corresponds to DIO1
#define rst 18 // This is the reset pin
#define gpio 15 // This is the GPIO pin. We set this to BUSY pin on the Teensy

#endif //CONFIG_H
