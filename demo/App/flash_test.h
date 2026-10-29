/*
 * flash_test.h - external flash self test at power up (sector 127, outside the
 * reserved device data): erase, blank check, program 16 pages, read back by DMA,
 * compare. Result and timings in g_flash_test; non-blocking, driven from the main loop.
 */
#ifndef FLASH_TEST_H
#define FLASH_TEST_H

#include <stdint.h>

/* Reads the chip ID and starts the test. After BspSpi1_Init. */
void FlashTest_Start(uint32_t now_ms);

/* Call from the main loop (also runs BspFlash_Poll) */
void FlashTest_Poll(uint32_t now_ms);

#endif /* FLASH_TEST_H */
