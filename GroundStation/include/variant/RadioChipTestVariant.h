#pragma once
#include "IVariant.h"

class RadioChipTestVariant : public IVariant
{
public:
    void setup() override;
    void loop() override;
};