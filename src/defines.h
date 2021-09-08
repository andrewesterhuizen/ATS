#pragma once

#define F_CPU 20000000UL

#define SAMPLE_RATE 32000
#define SAMPLES_PER_MS SAMPLE_RATE / 1000
#define MS_IN_MINUTE 60000

#define TAP_INPUT_PIN 6

#define BPM_POT_INPUT_PIN 0
#define FUNC_A_POT_INPUT_PIN 4
#define FUNC_B_POT_INPUT_PIN 5

#define NUM_OUTPUTS 5

// 24 / 24 = 1/4 24 / (24 / 24)
// 24 / 12 = 1/8; 24 / (24 / 2)
// 24 / 6 = 1/16; 24 / (24 / 4)

#define PPQN 24
#define PPQN_SIXTEENTHS 6

#define NUM_ENGINES 2