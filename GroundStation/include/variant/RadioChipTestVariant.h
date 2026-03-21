#pragma once
#include "IVariant.h"
#include <memory>

class RadioModule;

class RadioChipTestVariant : public IVariant
{
public:
    void setup() override;
    void loop() override;
private:
    std::unique_ptr<RadioModule> radioModule;
};