#pragma once

//Shared SPI
constexpr int MOSI_PIN = 11;
constexpr int MISO_PIN = 12;
constexpr int SCK_PIN = 13;

// Radios Specific SPI pins
constexpr int NSS_PIN = 10; // NSS is the CS for the Radio
constexpr int DIO1_PIN = 14;
constexpr int BUSY_PIN = 15;
constexpr int RST_PIN = 18;
constexpr int FREQ_PIN = 22;

// OLED Specifc SPI pins
// Lablling on OLED is misleading
// SCL → SCK
// SDA → MOSI
// CS  → CS
// DC  → any GPIO
// RES → any GPIO
constexpr int DISPLAY_CS_PIN   = 9;
constexpr int DISPLAY_DC_PIN   = 8;
constexpr int DISPLAY_RST_PIN  = 7;

// Joystick
constexpr int STICK_VRX_PIN = 19;
constexpr int STICK_VRY_PIN = 20;
constexpr int STICK_SW_PIN = 21;

