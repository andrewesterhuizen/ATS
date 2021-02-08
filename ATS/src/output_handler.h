#pragma once

#include <stdint.h>
#include <avr/io.h>

class OutputHandler
{
    int jackpins[5] = {0, 4, 1, 2, 3};
    int ledpins[5] = {7, 3, 0, 1, 2};

public:
    void updateOuput(uint8_t n, bool active)
    {
        if (active)
        {
            PORTD |= 1 << jackpins[n];
        }
        else
        {
            PORTD &= ~(1 << jackpins[n]);
        }
    }

    void updateLED(uint8_t n, bool active)
    {
        if (active)
        {
            // input 0 is on a different output port :(
            if (n == 0)
            {
                PORTD |= 1 << ledpins[n];
            }
            else
            {
                PORTC |= 1 << ledpins[n];
            }
        }
        else
        {
            if (n == 0)
            {
                PORTD &= ~(1 << ledpins[n]);
            }
            else
            {
                PORTC &= ~(1 << ledpins[n]);
            }
        }
    }
};