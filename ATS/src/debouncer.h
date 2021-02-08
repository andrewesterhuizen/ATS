#pragma once

#include <stdint.h>

class Debouncer
{
public:
    // true after 8ms because of shift register
    bool active = false;
    // stops counting after 255 ms
    uint8_t timeActiveMS = 0;

    Debouncer(bool inverted = false) : _inverted(inverted) {}

    void update(uint8_t value)
    {
        _shiftRegister = (_shiftRegister << 1) | value;
        active = _shiftRegister == (_inverted ? 0 : 0xff);

        if (active)
        {
            if (timeActiveMS == 0)
            {
                // account for debounce time
                timeActiveMS += _intervalMS * 8;
            }

            if (timeActiveMS < 0xff)
            {
                timeActiveMS += _intervalMS;
            }
        }
        else
        {
            timeActiveMS = 0;
        }
    };

private:
    uint8_t _shiftRegister = 0;
    uint8_t _intervalMS = 1;
    bool _inverted = false;
};