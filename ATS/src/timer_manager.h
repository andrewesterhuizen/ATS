#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <stdint.h>
#include "defines.h"

class TimerManager
{
public:
    void tick();
    bool nextBeat();
    void setBPM(uint32_t bpm, bool tap);
    void reset();

private:
    uint32_t _prevPotBPM = 240;
    uint32_t _bpm = 240;
    uint32_t _count = 0;
    uint32_t _samplesPerBeat = (MS_IN_MINUTE / _bpm) * SAMPLES_PER_MS; // (ms per beat / bpm) * samples per ms
};

void TimerManager::setBPM(uint32_t bpm, bool tap = false)
{

    if (tap)
    {
        _bpm = bpm;
    }
    else if (bpm != _prevPotBPM)
    {
        _prevPotBPM = _bpm;
        _bpm = bpm;
    }

    _samplesPerBeat = (MS_IN_MINUTE / _bpm) * SAMPLES_PER_MS;
}

void TimerManager::tick()
{
    _count++;
}

bool TimerManager::nextBeat()
{
    if (_count >= _samplesPerBeat)
    {
        _count = 0;
        return true;
    }

    return false;
}

void TimerManager::reset()
{
    _count = 0;
}

#endif // TIMER_MANAGER_H
