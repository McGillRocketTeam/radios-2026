#pragma once
#include <SPI.h>
#include "PinLayout.h"

// Returns the SPI bus the radio is wired to on this board.
//   Teensy: the default SPI bus (unchanged behaviour).
//   STM32F407 comms board: SPI3 on PC10/PC11/PC12.
inline SPIClass& radioSPI()
{
#if defined(BOARD_STM32F407)
    static SPIClass spi(RADIO_MOSI_PIN, RADIO_MISO_PIN, RADIO_SCK_PIN);
    return spi;
#else
    return SPI;
#endif
}