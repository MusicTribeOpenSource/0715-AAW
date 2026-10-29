/*
 * bsp_flash.h - W25X40CL SPI NOR flash (512 KB, 4 KB sectors, 256 byte pages) on SPI1,
 * CS PC7, shared with the OLED. Data phases by DMA (bsp_spi1), asynchronous:
 * a request is queued, BspFlash_Poll (main loop) runs it when the bus is free,
 * waits out program/erase (status polling, no busy wait) and calls done() in the
 * main loop. One request at a time.
 *
 * Sectors 0..79 are reserved for device data: presets, settings and the factory info
 * (sector 50: calibration). Program and erase refuse them.
 */
#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#include <stdint.h>

#define BSP_FLASH_SIZE              (512U * 1024U)
#define BSP_FLASH_SECTOR_SIZE       4096U
#define BSP_FLASH_PAGE_SIZE         256U
#define BSP_FLASH_FIRST_FREE_SECTOR 80U     /* first sector outside the reserved device data */

typedef void (*BspFlashDone_t)(int result);    /* 0 = done, < 0 = error */

/* Reads manufacturer / device ID (0x90): 0xEF12 for the W25X40CL. Polled, at init. */
uint16_t BspFlash_ReadId(void);

/* Queue a request; returns 0, or -1 if a request is pending or the arguments are invalid.
 * Buffers must stay valid until done(). Program: within one page. */
int BspFlash_Read(uint32_t addr, uint8_t *dst, uint32_t len, BspFlashDone_t done);
int BspFlash_Program(uint32_t addr, const uint8_t *src, uint32_t len, BspFlashDone_t done);
int BspFlash_EraseSector(uint32_t addr, BspFlashDone_t done);

/* A request is pending */
uint8_t BspFlash_Busy(void);

/* Call from the main loop */
void BspFlash_Poll(void);

#endif /* BSP_FLASH_H */
