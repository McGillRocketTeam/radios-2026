#include <Arduino.h>
#include "VariantFactory.h"

static IVariant& variant = getVariant();

void setup()
{
    variant.setup();
}

void loop()
{
    variant.loop();
}