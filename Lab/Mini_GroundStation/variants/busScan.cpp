#include <Wire.h>

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    while (!Serial)
        ;

    Serial.println("Scanning...");

    for (uint8_t addr = 1; addr < 127; addr++)
    {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0)
        {
            Serial.print("Found: 0x");
            Serial.println(addr, HEX);
        }
    }
}

void loop() {}