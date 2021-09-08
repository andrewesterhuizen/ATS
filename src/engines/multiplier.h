#pragma once

#include <stdint.h>

#include "../defines.h"
#include "./engine.h"

class MultiplierEngine : public Engine
{
    uint8_t shift = 0;
    uint8_t stretch = 1;

public:
    virtual void reset()
    {
    }

    virtual void setX(uint8_t value)
    {
        uint8_t scaledValue = ((value * 9) / 255) + 1; // between 1 and 10
        shift = scaledValue;
    }

    virtual void setY(uint8_t value)
    {
        uint8_t scaledValue = ((value * 9) / 255) + 1; // between 1 and 10
        stretch = scaledValue;
    }

    virtual void tick(uint8_t count, uint8_t *state)
    {
        for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
        {
            uint8_t div = ((i + 2) + shift) * stretch;
            uint8_t out = NUM_OUTPUTS - i - 1;
            if ((count % div) == 0)
            {
                (*state) |= 1 << out;
            }
            else
            {
                (*state) &= ~(1 << out);
            }
        }
    }
};
