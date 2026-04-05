#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

const int DISPLAY_SDA = 18;
const int DISPLAY_SCL = 19;
const int DISPLAY_RST = 17;

// Order for HW_I2C is: rotation, reset, clock, data
U8G2_SSD1309_128X64_NONAME0_F_HW_I2C display(
    U8G2_R0,
    DISPLAY_RST,
    DISPLAY_SCL,
    DISPLAY_SDA
);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();
  Wire.setClock(100000);

  display.setI2CAddress(0x3C << 1);
  display.begin();

  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);
  display.drawStr(10, 20, "Hello");
  display.sendBuffer();
}

void loop() {
}