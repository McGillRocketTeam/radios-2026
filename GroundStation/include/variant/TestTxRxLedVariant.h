#pragma once
#include "IVariant.h"

class TestTxRxLedVariant : public IVariant
{
public:
    void setup() override;
    void loop() override;
};