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
    InputY,
    Tap,
    ButtonRelease,
};

struct UIEvent
{
    uint8_t input;
    uint16_t value;
    bool shift;
};

class InputHandler
{
    ADC8 adc;

    Queue<UIEvent> eventQueue;

    Debouncer<true> tapButtonHandler;
    Debouncer<true> jackInputHandler;

    uint8_t lastX = 0;
    uint8_t lastY = 0;
    bool lastButtonState = false;
    bool lastJackState = false;

    uint8_t tapButtonReading = 0;
    uint8_t jackInputReading = 0;

    bool processTap = false;
    bool waitingForNextTap = false;
    uint16_t msSinceLastTap = 0;
    uint8_t tapJackReading = 0;

    uint8_t buttonPressCooldown = 0;

    bool buttonDownInLastUpdate = false;
    uint16_t timeSinceLastButtonRelease = 0;

public:
    void init()
    {
        adc.init();
    }

    void update()
    {
        tapButtonReading = (PIND >> 5) & 1;
        jackInputReading = (PIND >> 6) & 1;

        bool tapButtonActive = tapButtonHandler.update(tapButtonReading);
        bool jackInputActive = jackInputHandler.update(jackInputReading);

        timeSinceLastButtonRelease++;

        bool buttonReleased = buttonDownInLastUpdate && !tapButtonActive;
        if (buttonReleased)
        {
            eventQueue.enqueue(UIEvent{ButtonRelease, timeSinceLastButtonRelease + 8});
            timeSinceLastButtonRelease = 0;
        }

        buttonDownInLastUpdate = tapButtonActive;

        if (buttonPressCooldown > 0)
        {
            buttonPressCooldown--;
        }

        if (waitingForNextTap)
        {
            msSinceLastTap++;

            // max wait 2s
            if (msSinceLastTap > 2000)
            {
                waitingForNextTap = false;
                msSinceLastTap = 0;
            }
        }

        if (buttonPressCooldown == 0 && tapButtonActive && !lastButtonState)
        {
            buttonPressCooldown = 200;

            if (waitingForNextTap)
            {
                waitingForNextTap = false;
                uint16_t newBPM = MS_IN_MINUTE / msSinceLastTap;
                uint8_t newBPM8 = newBPM > 255 ? 255 : newBPM;
                eventQueue.enqueue(UIEvent{Tap, newBPM8});
            }
            else
            {
                waitingForNextTap = true;
                msSinceLastTap = 0;
            }
        }

        if (tapButtonActive != lastButtonState)
        {
            eventQueue.enqueue(UIEvent{InputButton, tapButtonActive});
            lastButtonState = tapButtonActive;
        }

        if (jackInputActive != lastJackState)
        {
            eventQueue.enqueue(UIEvent{InputJack, jackInputActive});
            lastJackState = jackInputActive;
        }

        uint8_t x = 255 - adc.read(FUNC_A_POT_INPUT_PIN);
        uint8_t y = 255 - adc.read(FUNC_B_POT_INPUT_PIN);

        if (x != lastX)
        {
            eventQueue.enqueue(UIEvent{InputX, x, tapButtonActive});
        }

        if (y != lastY)
        {
            eventQueue.enqueue(UIEvent{InputY, y, tapButtonActive});
        }

        lastX = x;
        lastY = y;
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
