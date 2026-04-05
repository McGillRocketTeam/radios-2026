#pragma once

// OLD vs NEW

// Radios
// constexpr int NSS_PIN = 10;
// constexpr int DIO1_PIN = 14;
// constexpr int BUSY_PIN = 15;
// constexpr int RST_PIN = 18;
// constexpr int FREQ_PIN = 22;
// constexpr int RX_LED_PIN = 23;
// constexpr int TX_LED_PIN = 19;

// Display
// constexpr int DISPLAY_SDA = 18;
// constexpr int DISPLAY_SCL = 19;
// constexpr int DISPLAY_RST = 17;

// Joystick
// constexpr int STICK_VRX_PIN = 14;
// constexpr int STICK_VRY_PIN = 15;
// constexpr int STICK_SW_PIN = 16;


// Harmonised New

// Radios untouched
constexpr int NSS_PIN = 10;
constexpr int DIO1_PIN = 14;
constexpr int BUSY_PIN = 15;
constexpr int RST_PIN = 18;
constexpr int FREQ_PIN = 22;

// Change the tx/rx leds
constexpr int RX_LED_PIN = 3;
constexpr int TX_LED_PIN = 4;

// Update to using i2c wire 2
constexpr int DISPLAY_SDA = 17;
constexpr int DISPLAY_SCL = 16;
// Reset doesnt need analog just gpio
constexpr int DISPLAY_RST = 9;

constexpr int STICK_VRX_PIN = 19;
constexpr int STICK_VRY_PIN = 20;
constexpr int STICK_SW_PIN = 21;




