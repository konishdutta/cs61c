#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lfsr.h"

void lfsr_calculate(uint16_t *reg) {
    /* YOUR CODE HERE */
    uint16_t orig = *reg;
    uint16_t c0 = orig & 1;
    orig = orig >> 2;
    uint16_t c2 = orig & 1;
    orig = orig >> 1;
    uint16_t c3 = orig & 1;
    orig = orig >> 2;
    uint16_t c5 = orig & 1;
    uint16_t c = c0 ^ c2 ^ c3 ^ c5;
    c = c << 15;
    *reg = *reg >> 1;
    *reg = c | *reg;
}

