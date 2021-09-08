#include "defines.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "pulse_generator.h"
#include "timer_manager.h"
#include "./module.h"
#include "./ui.h"
#include "./output_handler.h"
#include "./util.h"

// not sure why this fixes my issue
extern "C" void __cxa_pure_virtual()
{
    while (1)
        ;
}

PulseGenerator output_pulses[NUM_OUTPUTS];
TimerManager timer_manager;
Module m;
InputHandler input_handler;
OutputHandler output_handler;
bool ui_handled = true;

// every 1ms, used for reading from and writing to ui
ISR(TIMER0_COMPA_vect)
{
    ui_handled = false;
}

ISR(TIMER1_COMPA_vect)
{
    timer_manager.tick();

    // check if time to update clock
    if (timer_manager.next_beat())
    {
        m.tick();
        uint8_t output_state = m.state;

        for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
        {
            if (output_state & (1 << i))
            {
                output_pulses[i].trigger();
            }
        }
    }

    // output pulses to GPIO ports
    for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
    {
        output_handler.update_output(i, output_pulses[i].update());
    }
}

void init()
{
    // init io
    DDRC |= 0xff; // leds data direction
    DDRD |= 0xff; // jacks data direction

    DDRC &= ~(1 << FUNC_A_POT_INPUT_PIN);
    DDRC &= ~(1 << FUNC_B_POT_INPUT_PIN);

    DDRD &= ~(1 << 5); // switch
    PORTD |= (1 << 5); // enable pull up

    DDRD &= ~(1 << 6); // jack
    PORTD |= (1 << 6); // enable pull up

    input_handler.init();

    // init clock
    m.init();
    m.set_engine(0);

    cli();

    // init timer 0
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;
    OCR0A = 249; // compare register, 1khz, (16*10^6) / (1000*64) - 1
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1 << CS01) | (1 << CS00);
    TIMSK0 |= (1 << OCIE0A);

    // init timer 1
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << CS10) | (1 << WGM12);
    TCNT1 = 0;
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = (F_CPU / SAMPLE_RATE) - 1; // compare register, sample rate

    sei();
}

int main()
{
    init();

    while (1)
    {
        if (!ui_handled)
        {
            input_handler.update();

            for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
            {
                output_handler.update_led(i, output_pulses[i].is_active());
            }

            ui_handled = true;
        }

        // handle ui events
        while (input_handler.has_events())
        {
            UIEvent event = input_handler.get_event();
            switch (event.input)
            {
            case InputX:
                if (event.shift)
                {
                    timer_manager.set_bpm(event.value);
                }
                else
                {
                    m.set_x(event.value);
                }
                break;
            case InputY:
                if (event.shift)
                {
                    uint8_t engine_index = (event.value * NUM_ENGINES) / 255;
                    m.set_engine(engine_index);
                }
                else
                {
                    m.set_y(event.value);
                }
                break;
            case ButtonRelease:
                if (event.value > 0)
                {
                    uint16_t new_bpm = MS_IN_MINUTE / event.value;
                    uint8_t bpm = clamp(new_bpm);
                    timer_manager.set_bpm(bpm);
                }

                break;
            }
        }
    }
}