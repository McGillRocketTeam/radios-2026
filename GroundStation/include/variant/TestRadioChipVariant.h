#pragma once
#include "IVariant.h"

class TestRadioChipVariant : public IVariant
{
public:
    void setup() override;
    void loop() override;
};