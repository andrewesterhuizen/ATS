#pragma once

#include <stdint.h>
#include "defines.h"

class PulseGenerator
{
private:
    uint8_t _remaining = 0;
    uint8_t _pulseLengthMs = 100;
    uint8_t _pulseLengthSamples = (SAMPLE_RATE / 1000) * _pulseLengthMs;

public:
    bool is_active();
    bool update();
    void trigger();
};

void PulseGenerator::trigger()
{
    if (_pulseLengthSamples > _remaining)
    {
        _remaining = _pulseLengthSamples;
    }
}

bool PulseGenerator::update()
{
    if (_remaining > 0)
    {
        _remaining--;
        return true;
    }

    return false;
}

bool PulseGenerator::is_active()
{
    return _remaining > 0;
}
