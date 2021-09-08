#pragma once

#include <stdint.h>

template <bool inverted = false>
class Debouncer
{
public:
    uint8_t shift = 0;

    bool isActive()
    {
        if (!inverted)
        {
            return shift == 0xff;
        }

        return shift == 0;
    }

    bool update(uint8_t value)
    {
        shift = (shift << 1) | value;
        return isActive();
    };
};