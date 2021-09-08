#pragma once

#include "./engines/engine.h"
#include "./engines/divider.h"
#include "./engines/multiplier.h"
#include "./defines.h"

class Module
{
private:
    uint8_t count;
    uint8_t count_limit = PPQN * 10; // this could overflow if we update the ppqn
    uint8_t ppqn;

    uint8_t current_engine_index;
    Engine *engines[NUM_ENGINES];
    DividerEngine divider_engine;
    MultiplierEngine multiplier_engine;

    Engine *current_engine()
    {
        return engines[current_engine_index];
    }

public:
    uint8_t state;

    void init()
    {
        ppqn = PPQN;

        count = 0;
        state = 0;

        current_engine_index = 0;
        engines[0] = &divider_engine;
        engines[1] = &multiplier_engine;
    }

    void set_engine(uint8_t index)
    {
        current_engine_index = index;
    }

    void set_x(uint8_t value)
    {
        current_engine()->set_x(value);
    }

    void set_y(uint8_t value)
    {
        current_engine()->set_y(value);
    }

    void tick()
    {
        current_engine()->tick(count, &state);

        count++;
        if (count == (ppqn * 5))
        {
            count = 0;
        }
    }
};