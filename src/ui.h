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

    Queue<UIEvent> event_queue;

    Debouncer<true> tap_button_handler;
    Debouncer<true> jack_input_handler;

    uint8_t last_x = 0;
    uint8_t last_y = 0;
    bool last_button_state = false;
    bool last_jack_state = false;

    uint8_t tap_button_reading = 0;
    uint8_t jack_input_reading = 0;

    bool waiting_for_next_tap = false;
    uint16_t ms_since_last_tap = 0;

    uint8_t button_press_cooldown = 0;

    bool button_down_in_last_update = false;
    uint16_t time_since_last_button_release = 0;

public:
    void init()
    {
        adc.init();
    }

    void update()
    {
        tap_button_reading = (PIND >> 5) & 1;
        jack_input_reading = (PIND >> 6) & 1;

        bool tap_button_active = tap_button_handler.update(tap_button_reading);
        bool jack_input_active = jack_input_handler.update(jack_input_reading);

        time_since_last_button_release++;

        bool buttonReleased = button_down_in_last_update && !tap_button_active;
        if (buttonReleased)
        {
            event_queue.enqueue(UIEvent{ButtonRelease, time_since_last_button_release + 8});
            time_since_last_button_release = 0;
        }

        button_down_in_last_update = tap_button_active;

        if (button_press_cooldown > 0)
        {
            button_press_cooldown--;
        }

        if (waiting_for_next_tap)
        {
            ms_since_last_tap++;

            // max wait 2s
            if (ms_since_last_tap > 2000)
            {
                waiting_for_next_tap = false;
                ms_since_last_tap = 0;
            }
        }

        if (button_press_cooldown == 0 && tap_button_active && !last_button_state)
        {
            button_press_cooldown = 200;

            if (waiting_for_next_tap)
            {
                waiting_for_next_tap = false;
                uint16_t newBPM = MS_IN_MINUTE / ms_since_last_tap;
                uint8_t newBPM8 = newBPM > 255 ? 255 : newBPM;
                event_queue.enqueue(UIEvent{Tap, newBPM8});
            }
            else
            {
                waiting_for_next_tap = true;
                ms_since_last_tap = 0;
            }
        }

        if (tap_button_active != last_button_state)
        {
            event_queue.enqueue(UIEvent{InputButton, tap_button_active});
            last_button_state = tap_button_active;
        }

        if (jack_input_active != last_jack_state)
        {
            event_queue.enqueue(UIEvent{InputJack, jack_input_active});
            last_jack_state = jack_input_active;
        }

        uint8_t x = 255 - adc.read(FUNC_A_POT_INPUT_PIN);
        uint8_t y = 255 - adc.read(FUNC_B_POT_INPUT_PIN);

        if (x != last_x)
        {
            event_queue.enqueue(UIEvent{InputX, x, tap_button_active});
        }

        if (y != last_y)
        {
            event_queue.enqueue(UIEvent{InputY, y, tap_button_active});
        }

        last_x = x;
        last_y = y;
    }

    bool has_events()
    {
        return !event_queue.is_empty();
    }

    UIEvent get_event()
    {
        return event_queue.dequeue();
    }
};
