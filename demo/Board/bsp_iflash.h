/*
 * bsp_iflash.h - internal flash (FMC) erase / program, asynchronous
 * (fmc_sector_erase_split_1 / _2: start the erase, poll for the end).
 *
 * Only sector 5 (0x08020000, 128 KB) may be changed: sectors 0..3 = bootloader,
 * 4 = this image, 6..7 = reserved device presets / settings. Refused when the
 * image has grown into sector 5. Read: the flash is memory mapped, read it directly.
 */
#ifndef BSP_IFLASH_H
#define BSP_IFLASH_H

#include <stdint.h>

#define BSP_IFLASH_FREE_ADDR    0x08020000U     /* sector 5 */
#define BSP_IFLASH_FREE_SIZE    0x00020000U     /* 128 KB */

/* Start erasing sector 5; returns 0, or -1 when busy or the image reaches into sector 5 */
int BspIFlash_EraseStart(void);

/* Start programming nwords 32 bit words (word aligned, inside sector 5, erased before);
 * words must stay valid until done. Returns 0 or -1. */
int BspIFlash_ProgramStart(uint32_t addr, const uint32_t *words, uint32_t nwords);

/* An operation is running */
uint8_t BspIFlash_Busy(void);

/* Result of the last operation: 0 = ok, < 0 = FMC error */
int BspIFlash_Result(void);

/* Call from the main loop: ends the erase, programs up to 32 words per call */
void BspIFlash_Poll(void);

#endif /* BSP_IFLASH_H */
