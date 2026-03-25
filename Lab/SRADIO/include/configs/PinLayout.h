#pragma once
/*
 * -------------------- Connections to Teensy 4.0 --------------------
 *
 * Teensy Pins:
 *  Pin 10  -> NSS / Chip select
 *  Pin 13  -> SPI Serial Clock (SCK)
 *  Pin 14  -> Digital I/O 1 (DIO1 – Interrupt Output from SX1262)
 *  Pin 15  -> Busy Status Output (BUSY)
 *  Pin 18  -> Hardware Reset (NRESET)
 *  Pin 22  -> Frequency Reference / TCXO Control (FREQ_RES / DIO3)
 *  Pin 24  -> 3.3 V Power Supply (3V3)
 *
 * ------------------------------------------------------------------
 * ----------------------- SX1262 Breakout ---------------------------
 * ------------------------------------------------------------------
 *
 * SPI Interface:
 *  RX1/TXD -> SPI Master Out, Slave In (MOSI)  [MCU → SX1262]
 *  TX1/RXD -> SPI Master In, Slave Out (MISO)  [SX1262 → MCU]
 *  NSS     -> SPI Chip Select (NSS / CS)
 *
 * Control & Status:
 *  DIO1    -> Digital Interrupt Output 1 (TxDone, RxDone, IRQs)
 *  BUSY    -> Busy Status Indicator (radio ready/busy flag)
 *  NRESET  -> Hardware Reset Input (active low)
 *
 * Clock & Power:
 *  FREQ_RES-> Frequency Reference / TCXO Enable (via DIO3)
 *  3V3     -> Power Supply Input (VDD)
 *  GND     -> Ground
 *
 * ------------------------------------------------------------------
 */

constexpr int NSS_PIN  = 10;
constexpr int DIO1_PIN = 14;
constexpr int BUSY_PIN = 15;
constexpr int RST_PIN  = 18;
constexpr int FREQ_PIN = 22;
