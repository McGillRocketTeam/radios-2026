#include <Arduino.h>
#include "MiniGroundStation.h"

static MiniGroundStation miniGroundStation;

void setup()
{
    miniGroundStation.setup();
}

void loop()
{
    miniGroundStation.poll();
}
