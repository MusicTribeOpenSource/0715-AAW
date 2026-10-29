/*
 * bsp_buttons.c - 74HC165 chain read, see bsp_buttons.h
 * Timing: 100 ns after PL, 10 ns clock high/low, 50 us after the read.
 */
#include "bsp_buttons.h"
#include "bsp_delay.h"
#include "bsp_gpio.h"

/* Compile time constant: a runtime conversion from SystemCoreClock costs a 64 bit
 * division (~1700 cycles) per call, 257 calls per read. Rounded up (minimum delay). */
#define CORE_CLOCK_MHZ      120U    /* must match SystemInit (system_gd32f4xx.c, 120 MHz) */
#define NS_TO_CYCLES(ns)    ((((ns) * CORE_CLOCK_MHZ) + 999U) / 1000U)

void BspButtons_Read(uint8_t regs[BSP_BUTTONS_NUM_SHIFT_REGS])
{
    uint32_t r, b;

    gpio_bit_set(BSP_PIN_SCAN_PL);
    BspDelay_Cycles(NS_TO_CYCLES(100));

    for (r = 0; r < BSP_BUTTONS_NUM_SHIFT_REGS; r++)
    {
        uint8_t value = 0;
        for (b = 0; b < 8U; b++)
        {
            value <<= 1;
            value |= (uint8_t)gpio_input_bit_get(BSP_PIN_SCAN_IN);
            gpio_bit_set(BSP_PIN_SCAN_CLK);
            BspDelay_Cycles(NS_TO_CYCLES(10));
            gpio_bit_reset(BSP_PIN_SCAN_CLK);
            BspDelay_Cycles(NS_TO_CYCLES(10));
        }
        regs[r] = value;
    }

    gpio_bit_reset(BSP_PIN_SCAN_PL);
    BspDelay_Us(50U);
}
