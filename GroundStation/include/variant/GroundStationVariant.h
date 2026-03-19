#pragma once
#include "IVariant.h"

class GroundStationVariant : public IVariant
{
public:
    void setup() override;
    void loop() override;
};