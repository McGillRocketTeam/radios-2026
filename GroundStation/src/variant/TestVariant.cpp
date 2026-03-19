#include "TestVariant.h"
#include <Arduino.h>

void TestVariant::setup()
{
    Serial.begin(115200);
    Serial.println("Testing mode setup");
}

void TestVariant::loop()
{
    Serial.println("Testing mode loop");
    delay(1000);
}