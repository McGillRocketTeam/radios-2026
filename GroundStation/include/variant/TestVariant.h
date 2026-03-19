#pragma once
#include "IVariant.h"

class TestVariant : public IVariant
{
public:
    void setup() override;
    void loop() override;
};