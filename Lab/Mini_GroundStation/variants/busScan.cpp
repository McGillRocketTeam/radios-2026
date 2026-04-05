#include <Wire.h>

void setup()
{
    Serial.begin(115200);
    Wire1.begin();

    while (!Serial)
        ;

    Serial.println("Scanning...");

    for (uint8_t addr = 1; addr < 127; addr++)
    {
        Wire1.beginTransmission(addr);
        if (Wire1.endTransmission() == 0)
        {
            Serial.print("Found: 0x");
            Serial.println(addr, HEX);
        }
    }
}

void loop() {}