/*
 * bsp_delay.c - busy wait on the SysTick counter, see bsp_delay.h
 */
#include "bsp_delay.h"
#include "gd32f4xx.h"

void BspDelay_Cycles(uint32_t cycles)
{
    const uint32_t reload = SysTick->LOAD + 1U;
    uint32_t old = SysTick->VAL;
    uint32_t waited = 0;

    while (waited < cycles)
    {
        const uint32_t now = SysTick->VAL;

        /* SysTick counts down and wraps to LOAD */
        waited += (old >= now) ? (old - now) : (old - now + reload);
        old = now;
    }
}

void BspDelay_Us(uint32_t us)
{
    BspDelay_Cycles(us * (SystemCoreClock / 1000000U));
}
