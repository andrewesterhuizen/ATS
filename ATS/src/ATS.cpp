// #include "Arduino.h"
#include "defines.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc8.h"
#include "clock.h"
#include "pulse_generator.h"
#include "timer_manager.h"

ADC8 adc;
Clock clock;
PulseGenerator outputPulses[NUM_OUTPUTS];

TimerManager timerManager;

uint8_t clockOutputStateBit = 0;
uint8_t tapButtonReading = 0;

uint8_t tapButtonShiftRegister = 0;
uint8_t tapButtonDown = 0;
uint16_t tabButtonHeldTime = 0;

bool processTap = 0;
bool waitingForNextTap = 0;

uint32_t tapCounter = 0;
uint8_t tapJackReading = 0;

int ledpins[] = {
    7,
    3,
    0,
    1,
    2,
};

int jackpins[] = {
    0,
    4,
    1,
    2,
    3,
};

// ui polling
ISR(TIMER0_COMPA_vect)
{
    // disabled until board fixed in revision
    // timerManager.setBPM(adc.read(BPM_POT_INPUT_PIN) * 10);

    // clock.rotate = adc.read(FUNC_A_POT_INPUT_PIN) / 64;
    // clock.multiply = (adc.read(FUNC_B_POT_INPUT_PIN) / 64) + 1;

    // TODO: clean up this tap / switch state logic logic

    tapButtonReading = (~(bitRead(PIND, 5))) & (1 << 0);
    tapJackReading = (~(bitRead(PIND, 6))) & (1 << 0);

    tapButtonShiftRegister = (tapButtonShiftRegister << 1) | tapButtonReading | tapJackReading;

    tapButtonDown = tapButtonShiftRegister == 255;

    if (tapButtonDown)
    {
        tabButtonHeldTime++;
    }
    else
    {
        processTap = tabButtonHeldTime > 10;
        tabButtonHeldTime = 0;
    }

    if (processTap)
    {
        processTap = 0;

        if (waitingForNextTap)
        {
            waitingForNextTap = 0;
            timerManager.setBPM(MS_IN_MINUTE / tapCounter, true);
        }
        else
        {
            waitingForNextTap = 1;
            tapCounter = 0;
        }
    }

    if (waitingForNextTap)
    {
        tapCounter++;

        // max wait 2s
        if (tapCounter > 2000)
        {
            waitingForNextTap = 0;
            tapCounter = 0;
        }
    }
}

ISR(TIMER1_COMPA_vect)
{
    timerManager.tick();

    // check if time to update clock
    if (timerManager.nextBeat())
    {
        clockOutputStateBit = clock.tick();

        for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
        {
            if (clockOutputStateBit & (1 << i))
            {
                outputPulses[i].trigger();
            }
        }
    }

    // output pulses to GPIO ports
    for (uint8_t i = 0; i < NUM_OUTPUTS; i++)
    {
        if (outputPulses[i].update())
        {
            PORTD |= 1 << jackpins[i];
            PORTC |= 1 << ledpins[i];
        }
        else
        {
            PORTD &= ~(1 << jackpins[i]);
            PORTC &= ~(1 << ledpins[i]);
        }
    }
}

int main()
{
    // init io
    DDRC |= 0xff; // leds data direction
    DDRD |= 0xff; // jacks data direction

    DDRD &= ~(1 << 5); // switch
    PORTD |= (1 << 5); // enable pull up

    DDRD &= ~(1 << 6); // tap jack data direction to output
    PORTD |= (1 << 6); // enable pull up

    adc.init();

    // init clock
    clock.init(NUM_OUTPUTS);

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

    while (1)
    {
    }
}