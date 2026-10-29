/*
 * iflash_test.h - internal flash self test at power up (sector 5, 0x08020000):
 * erase, blank check of the sector, program 4 KB, read back and compare.
 * Also checks whether the CPU stalls during the erase: SysTick ms (interrupt driven)
 * against the DWT cycle counter. Result in g_iflash_test; non-blocking, from the main loop.
 */
#ifndef IFLASH_TEST_H
#define IFLASH_TEST_H

#include <stdint.h>

void IFlashTest_Start(uint32_t now_ms);

/* Call from the main loop (also runs BspIFlash_Poll) */
void IFlashTest_Poll(uint32_t now_ms);

#endif /* IFLASH_TEST_H */
