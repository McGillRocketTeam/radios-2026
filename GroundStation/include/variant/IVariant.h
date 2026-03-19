#pragma once

class IVariant
{
public:
    virtual ~IVariant() = default;
    virtual void setup() = 0;
    virtual void loop() = 0;
};