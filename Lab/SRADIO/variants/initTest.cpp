#include <Arduino.h>

#include <cassert>

#include "Config.h"
#include "RadioModule.h"
// ======================================================== 
// Variant of main.cpp that runs the main GS radio code
// 

static RadioModule *radioModule = nullptr;

void setup() {
    Serial.begin(GS_SERIAL_BAUD_RATE);
    while (!Serial)
    {
    }

    radioModule = new RadioModule();
}

void loop() {
    delay(20);
}
