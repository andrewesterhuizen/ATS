#include "defines.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "pulse_generator.h"
#include "timer_manager.h"
#include "./module.h"
#include "./ui.h"
#include "./output_handler.h"

// not sure why this fixes my issue
extern "C" void __cxa_pure_virtual()
{
    while (1)
        ;
}

PulseGenerator outputPulses[NUM_OUTPUTS];
TimerManager timerManager;
Module m;
InputHandler inputHandler;
OutputHandler outputHandler;
bool uiHandled = true;

// every 1ms, used for reading from and writing to ui
ISR(TIMER0_COMPA_vect)
{
    uiHandled = false;
}

ISR(TIMER1_COMPA_vect)
{
    timerManager.tick();

    // check if time to update clock
    if (timerManager.nextBeat())
    {
        m.tick();
        uint8_t moduleState = m.state;

        for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
        {
            if (moduleState & (1 << i))
            {
                outputPulses[i].trigger();
            }
        }
    }

    // output pulses to GPIO ports
    for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
    {
        outputHandler.updateOuput(i, outputPulses[i].update());
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

    inputHandler.init();

    // init clock
    m.init();
    m.setEngine(0);

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
        if (!uiHandled)
        {
            inputHandler.update();

            for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
            {
                outputHandler.updateLED(i, outputPulses[i].isActive());
            }

            uiHandled = true;
        }

        // handle ui events
        while (inputHandler.hasEvents())
        {
            UIEvent event = inputHandler.getEvent();
            switch (event.input)
            {
            case InputX:
                m.setX(event.value);
                break;
            case InputY:
                m.setY(event.value);
                break;
            }
        }
    }
}