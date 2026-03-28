#pragma once
#include "IVariant.h"

class TxRxLedTestVariant : public IVariant
{
public:
    void setup() override;
    void loop() override;
};