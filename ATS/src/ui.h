#pragma once

#include <avr/io.h>
#include "./adc8.h"
#include <stdint.h>
#include "./queue.h"
#include "./debouncer.h"

enum
{
    InputJack,
    InputButton,
    InputX,
    InputY
};

enum
{
    EventTypeActive,
    EventTypeNotActive,
    EventTypeChanged
};

struct UIEvent
{
    uint8_t input;
    uint8_t value;
};

class InputHandler
{
    ADC8 adc;

    Queue<UIEvent> eventQueue;

    Debouncer tapButtonHandler = Debouncer(true);
    Debouncer jackInputHandler = Debouncer(true);

    uint8_t shiftActive = false;

    uint8_t lastX = 0;
    uint8_t lastY = 0;

    uint8_t tapButtonReading = 0;
    uint8_t jackInputReading = 0;

    bool processTap = 0;
    bool waitingForNextTap = 0;

    uint32_t msSinceLastTap = 0;
    uint8_t tapJackReading = 0;

public:
    void init()
    {
        adc.init();
    }

    void update()
    {
        tapButtonReading = (PIND >> 5) & 1;
        jackInputReading = (PIND >> 6) & 1;

        tapButtonHandler.update(tapButtonReading);
        jackInputHandler.update(jackInputReading);

        uint8_t x = 255 - adc.read(FUNC_A_POT_INPUT_PIN);
        uint8_t y = 255 - adc.read(FUNC_B_POT_INPUT_PIN);

        if (x != lastX)
        {
            eventQueue.enqueue(UIEvent{InputX, x});
        }

        if (y != lastY)
        {
            eventQueue.enqueue(UIEvent{InputY, y});
        }

        lastX = x;
        lastY = y;

        if (tapButtonHandler.active)
        {
            // uint8_t engineIndex = (x * NUM_ENGINES) / 255;
            // m.setEngine(engineIndex);
            // shiftOccured = true;
        }
        else
        {
            // m.setX(x);
        }

        if (tapButtonHandler.active)
        {
            // timerManager.setBPM(y, false);
            // shiftOccured = true;
        }
        else
        {
            // m.setY(y);
        }

        // don't do tap tempo logic if button press was to use a shifted input
        // if (shiftOccured)
        // {
        //     shiftOccured = false;
        //     return;
        // }

        // bool processTapButton = tapButtonHandler.active && tapButtonHandler.timeActiveMS > 20;
        // bool processTapJack = jackInputHandler.active && jackInputHandler.timeActiveMS > 20;

        // processTap = processTapButton || processTapJack;

        // if (processTap)
        // {
        //     processTap = false;

        //     if (waitingForNextTap)
        //     {
        //         waitingForNextTap = 0;
        //         timerManager.setBPM(MS_IN_MINUTE / msSinceLastTap, true);
        //     }
        //     else
        //     {
        //         waitingForNextTap = 1;
        //         msSinceLastTap = 0;
        //     }
        // }

        // if (waitingForNextTap)
        // {
        //     msSinceLastTap++;

        //     // max wait 2s
        //     if (msSinceLastTap > 2000)
        //     {
        //         waitingForNextTap = 0;
        //         msSinceLastTap = 0;
        //     }
        // }
    }

    bool hasEvents()
    {
        return !eventQueue.isEmpty();
    }

    UIEvent getEvent()
    {
        return eventQueue.dequeue();
    }
};
