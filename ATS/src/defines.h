#ifndef DEFINES_H
#define DEFINES_H

#define F_CPU 20000000UL

#define SAMPLE_RATE 16000
#define SAMPLES_PER_MS SAMPLE_RATE / 1000
#define MS_IN_MINUTE 60000

#define TAP_INPUT_PIN 6

#define BPM_POT_INPUT_PIN 0
#define FUNC_A_POT_INPUT_PIN 1
#define FUNC_B_POT_INPUT_PIN 2

#define NUM_OUTPUTS 5

// TODO: remove this
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

#endif // DEFINES_H
