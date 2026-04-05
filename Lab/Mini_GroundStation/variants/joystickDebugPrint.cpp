#include <Arduino.h>

const int VRX_PIN = 14;
const int VRY_PIN = 15;
const int SW_PIN = 16;

void setup()
{
    Serial.begin(115200);
    // No resistor on joystick means we need this Input pull up
    // Supplying 3.3 volts makes the reads cleaner
    pinMode(SW_PIN, INPUT_PULLUP);
}

void loop()
{
    int vrx = analogRead(VRX_PIN);
    int vry = analogRead(VRY_PIN);
    int buttonState = digitalRead(SW_PIN);

    Serial.print("VRx: ");
    Serial.print(vrx);

    Serial.print(" | VRy: ");
    Serial.print(vry);

    Serial.print(" | Button: ");
    if (buttonState == LOW)
    {
        Serial.println("PRESSED");
    }
    else
    {
        Serial.println("RELEASED");
    }

    delay(200);
}