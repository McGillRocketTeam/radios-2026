#include <Arduino.h>
#include "Joystick.h"
#include "peripheralPins.h"

Joystick joystick(STICK_VRX_PIN, STICK_VRY_PIN, STICK_SW_PIN);


void setup()
{
    Serial.begin(115200);
    joystick.begin();
}

void loop()
{
    joystick.update();

    Joystick::Event event;
    if (joystick.getInput(event))
    {
        Serial.print("New direction: ");
        Serial.print(joystick.eventToString(event));
    }
}