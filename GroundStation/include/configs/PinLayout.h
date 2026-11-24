#pragma once
#include <Arduino.h>
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

#if defined(TEENSYDUINO)
    constexpr int NSS_PIN = 10;
    constexpr int DIO1_PIN = 14;
    constexpr int BUSY_PIN = 15;
    constexpr int RST_PIN = 18;
    constexpr int FREQ_PIN = 22;
    constexpr int RX_LED_PIN = 23;
    constexpr int TX_LED_PIN = 19;

/*
 * Connections to STM32 
 *
 */
#elif defined(BOARD_STM32F407)
    constexpr int NSS_PIN = PD2;
    constexpr int DIO1_PIN = PD4;
    constexpr int BUSY_PIN = PD3;
    constexpr int RST_PIN = PD5;
    constexpr int FREQ_PIN = PA3;
    constexpr int RX_LED_PIN = PB8;
    constexpr int TX_LED_PIN = PB9;

    // spi pins
    constexpr int RADIO_SCK_PIN  = PC10;   
    constexpr int RADIO_MISO_PIN = PC11;   
    constexpr int RADIO_MOSI_PIN = PC12;   

#else
    #error "No pin layout defined for this board"

#endif
