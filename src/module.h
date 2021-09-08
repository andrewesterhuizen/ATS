#pragma once

#include "./engines/engine.h"
#include "./engines/divider.h"
#include "./engines/multiplier.h"
#include "./defines.h"

class Module
{
private:
    uint8_t count;
    uint8_t countLimit = PPQN * 10; // this could overflow if we update the ppqn
    uint8_t ppqn;

    uint8_t currentEngineIndex;
    Engine *engines[NUM_ENGINES];
    DividerEngine dividerEngine;
    MultiplierEngine multiplierEngine;

    Engine *currentEngine()
    {
        return engines[currentEngineIndex];
    }

public:
    uint8_t state;

    void init()
    {
        ppqn = PPQN;

        count = 0;
        state = 0;

        currentEngineIndex = 0;
        engines[0] = &dividerEngine;
        engines[1] = &multiplierEngine;
    }

    void setEngine(uint8_t index)
    {
        currentEngineIndex = index;
    }

    void setX(uint8_t value)
    {
        currentEngine()->setX(value);
    }

    void setY(uint8_t value)
    {
        currentEngine()->setY(value);
    }

    void tick()
    {
        currentEngine()->tick(count, &state);

        count++;
        if (count == (ppqn * 5))
        {
            count = 0;
        }
    }
};