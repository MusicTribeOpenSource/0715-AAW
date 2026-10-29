/*
 * bsp_delay.h - busy wait on the SysTick counter
 * Needs SysTick running: valid after SysTick_Config() in main(). Only for short waits
 * (us range); use the 1 ms tick of main.c for anything near a tick.
 */
#ifndef BSP_DELAY_H
#define BSP_DELAY_H

#include <stdint.h>

void BspDelay_Cycles(uint32_t cycles);
void BspDelay_Us(uint32_t us);

#endif /* BSP_DELAY_H */
