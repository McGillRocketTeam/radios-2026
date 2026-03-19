#pragma once
#include "IVariant.h"

class FlightComputerVariant : public IVariant
{
public:
    void setup() override;
    void loop() override;
};