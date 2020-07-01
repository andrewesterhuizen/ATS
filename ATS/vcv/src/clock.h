#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdio.h>

enum FunctionID
{
    DIVIDER_FUNCION,
    NUM_CLOCK_FUNCTION
};

class Divider
{
private:
    uint8_t _state = 0;
    uint8_t count = 0;
    uint8_t _count16 = 0;
    uint8_t _numOutputs = 5;
    uint8_t rotate = 0;
    uint8_t multiply = 1;

public:
    uint8_t tick();
};

uint8_t Divider::tick()
{
    count++;

    // 16ths
    if (count == 6)
    {
        count = 0;
        _count16++;

        for (uint8_t i = 0; i < _numOutputs; i++)
        {
            uint8_t div = ((i + 1) + rotate) * multiply;

            if ((_count16 % div) == 0)
            {
                _state |= 1 << i;
            }
            else
            {
                _state &= ~(1 << i);
            }
        }

        return _state;
    }

    return 0;
}

class Multiplier
{
private:
    uint8_t _state = 0;
    uint8_t count = 0;
    uint8_t _count16 = 0;
    uint8_t _numOutputs = 5;
    uint8_t rotate = 0;
    uint8_t multiply = 1;

public:
    uint8_t tick();
};

uint8_t Multiplier::tick()
{
    count++;

    if (count == 2)
    {
        count = 0;
        _count16++;

        for (uint8_t i = 0; i < _numOutputs; i++)
        {
            // uint8_t div = ((i + 1) + rotate) * multiply;

            if ((_count16 % i + 1) == 0)
            {
                _state |= 1 << i;
            }
            else
            {
                _state &= ~(1 << i);
            }
        }

        return _state;
    }

    return 0;
}

class Clock
{
private:
    uint8_t _numOutputs = 5;
    Multiplier _function;

public:
    void
    init(uint8_t numOutputs);
    uint8_t tick();
};

void Clock::init(uint8_t numOutputs)
{
    printf("init\n");
    _numOutputs = numOutputs <= 5 ? numOutputs : 5;
};

uint8_t Clock::tick()
{
    printf("tick\n");

    uint8_t s = _function.tick();

    printf("out:");

    for (int i = 0; i < 5; i++)
    {
        printf("%d", (s >> (4 - i)) & 1);
    }
    printf("\n");
    return s;
}

#endif // CLOCK_H
