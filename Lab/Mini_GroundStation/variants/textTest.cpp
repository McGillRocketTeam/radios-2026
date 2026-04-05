#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "peripheralPins.h"

// Order for HW_I2C is: rotation, reset, clock, data
U8G2_SSD1309_128X64_NONAME0_F_HW_I2C display(
    U8G2_R0,
    DISPLAY_RST,
    DISPLAY_SCL,
    DISPLAY_SDA);

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Wire.begin();
    Wire.setClock(100000);

    display.setI2CAddress(0x3C << 1);
    display.begin();

    char line1[32];
    char line2[32];

    float lat = 145.501689f;
    float lon = -173.567256f;

    snprintf(line1, sizeof(line1), "GPS Lat %4.6f", lat);
    snprintf(line2, sizeof(line2), "GPS Lon %4.6f", lon);

    display.clearBuffer();
    display.setFont(u8g2_font_6x12_tr);
    display.drawStr(0, 10, line1);
    display.drawStr(0, 20, line2);
    display.sendBuffer();

    display.sendBuffer();
}

void loop()
{
}