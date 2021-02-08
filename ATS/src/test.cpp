#include "defines.h"
#include <iostream>
#include "./module.h"

#define ITERATIONS 120

uint8_t prevStates[NUM_OUTPUTS][ITERATIONS];

void updateStorage(int it, uint8_t state)
{
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        prevStates[i][it] = (state >> i) & 1;
    }
}

void printState()
{
    int c = 0;
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        for (int it = 0; it < ITERATIONS; it++)
        {
            int x = prevStates[i][it];
            if (x == 1)
            {
                printf("%d", 1);
            }
            else
            {
                printf("-");
            }

            c++;
            if (c == 6)
            {
                c = 0;
                printf(" ");
            }
        }
        c = 0;
        printf("\n");
    }

    printf("\n");
}

int main()
{
    Module m;
    m.init();

    for (uint8_t i = 0; i < ITERATIONS; i++)
    {
        m.tick();
        updateStorage(i, m.state);
    }

    printState();

    return 0;
}