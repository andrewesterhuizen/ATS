#pragma once
#include "../defines.h"

class Engine
{
public:
    Engine() {}
    ~Engine() {}
    virtual void reset() = 0;
    virtual void setX(uint8_t value) = 0;
    virtual void setY(uint8_t value) = 0;
    virtual void tick(uint8_t count, uint8_t *state) = 0;
};
