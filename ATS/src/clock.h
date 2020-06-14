#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

class Clock
{
private:
    uint8_t _numOutputs = 5;
    uint8_t _count = 0;
    uint8_t _state = 0;

public:
    uint8_t rotate = 0;
    uint8_t multiply = 1;
    void init(uint8_t numOutputs);
    uint8_t tick();
};

void Clock::init(uint8_t numOutputs)
{
    _numOutputs = numOutputs <= 5 ? numOutputs : 5;
};

uint8_t Clock::tick()
{
    for (uint8_t i = 0; i < _numOutputs; i++)
    {
        uint8_t div = ((i + 1) + rotate) * multiply;

        if ((_count % div) == 0)
        {
            _state |= 1 << i;
        }
        else
        {
            _state &= ~(1 << i);
        }
    }

    _count++;

    return _state;
}

#endif // CLOCK_H
