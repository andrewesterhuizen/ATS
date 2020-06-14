#ifndef PULSE_GENERATOR_H
#define PULSE_GENERATOR_H

#include <stdint.h>
#include "defines.h"

class PulseGenerator
{
private:
    uint8_t _remaining = 0;
    uint8_t _pulseLengthMs = 10;
    uint8_t _pulseLengthSamples = (SAMPLE_RATE / 1000) * _pulseLengthMs;

public:
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

#endif // PULSE_GENERATOR_H
