#pragma once

#include <stdint.h>
#include <avr/io.h>

class OutputHandler
{
    int jack_pins[5] = {0, 4, 1, 2, 3};
    int led_pins[5] = {7, 3, 0, 1, 2};

public:
    void update_output(uint8_t n, bool active)
    {
        if (active)
        {
            PORTD |= 1 << jack_pins[n];
        }
        else
        {
            PORTD &= ~(1 << jack_pins[n]);
        }
    }

    void update_led(uint8_t n, bool active)
    {
        if (active)
        {
            // input 0 is on a different output port :(
            if (n == 0)
            {
                PORTD |= 1 << led_pins[n];
            }
            else
            {
                PORTC |= 1 << led_pins[n];
            }
        }
        else
        {
            if (n == 0)
            {
                PORTD &= ~(1 << led_pins[n]);
            }
            else
            {
                PORTC &= ~(1 << led_pins[n]);
            }
        }
    }
};