#pragma once

#include <stdint.h>
#include "defines.h"

class TimerManager
{
public:
    void tick();
    bool next_beat();
    void set_bpm(uint32_t bpm);
    void reset();

private:
    uint32_t bpm = 120;
    uint32_t count = 0;
    uint32_t samples_per_beat = (MS_IN_MINUTE * SAMPLES_PER_MS) / bpm; // (ms per beat / bpm) * samples per ms
    uint32_t samples_per_tick = ((MS_IN_MINUTE * SAMPLES_PER_MS) / bpm) / PPQN;
};

void TimerManager::set_bpm(uint32_t bpm)
{
    bpm = bpm;
    if (bpm == 0)
    {
        bpm = 1;
    }

    samples_per_beat = (MS_IN_MINUTE * SAMPLES_PER_MS) / bpm;
    samples_per_tick = samples_per_beat / PPQN;

    reset();
}

void TimerManager::tick()
{
    count++;
}

bool TimerManager::next_beat()
{
    if (count >= samples_per_tick)
    {
        count = 0;
        return true;
    }

    return false;
}

void TimerManager::reset()
{
    count = 0;
}
