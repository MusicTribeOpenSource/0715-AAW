/*
 * bsp_buttons.h - buttons and encoder switches: chain of 16x 74HC165, bit banged
 * Hardware part only; the bit -> button/encoder mapping is in the application (input.c).
 */
#ifndef BSP_BUTTONS_H
#define BSP_BUTTONS_H

#include <stdint.h>

#define BSP_BUTTONS_NUM_SHIFT_REGS  16U

/* Clock the whole chain in, MSB first per register; takes ~60 us (busy wait) */
void BspButtons_Read(uint8_t regs[BSP_BUTTONS_NUM_SHIFT_REGS]);

#endif /* BSP_BUTTONS_H */
