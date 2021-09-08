#pragma once

#include <stdint.h>
#include <avr/io.h>

class ADC8
{
public:
    void init();
    uint8_t read(uint8_t pin);
};

void ADC8::init()
{
    ADMUX |= (1 << REFS0);                                // reference voltage on AVCC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // clock prescaler /128
    ADCSRA |= (1 << ADEN);                                // enable adc
    ADMUX |= (1 << ADLAR);                                // left align, for 8 bit mode
}

uint8_t ADC8::read(uint8_t pin)
{
    ADMUX = (0xf0 & ADMUX) | pin; // clear prev mux bits and set mux pin
    ADCSRA |= (1 << ADSC);        // start adc read

    while (ADCSRA & (1 << ADSC)) // wait until read is complete
    {
    }

    return ADCH;
}
