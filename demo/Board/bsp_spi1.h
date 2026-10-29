/*
 * bsp_spi1.h - SPI1 master, shared by the OLED (SSD1306) and the SPI flash (W25X40CL)
 * Polled transfers plus DMA transfers: DMA0 channel 4 = TX, channel 3 = RX
 * (sub-peripheral 0).
 * Bus ownership: polled accesses and the start of a DMA transfer happen in the main
 * loop only, while !BspSpi1_DmaBusy(). When a DMA transfer ends, its done() runs and
 * then the idle hook (OLED: start its pending frame), both in the DMA interrupt.
 */
#ifndef BSP_SPI1_H
#define BSP_SPI1_H

#include <stdint.h>

/* Mode 0, MSB first, PCLK1 (30 MHz) / 4 = 7.5 MHz for OLED and flash (SSD1306 max
 * 10 MHz; / 2 = 15 MHz would exceed it); DMA channels set up */
void BspSpi1_Init(void);

/* Polled write, returns when the last byte is in the shifter;
 * the received bytes are dropped. Must not be called while BspSpi1_DmaBusy(). */
void BspSpi1_Write(const uint8_t *data, uint32_t len);

/* Polled full duplex byte: sends byte, returns the byte received meanwhile.
 * Must not be called while BspSpi1_DmaBusy(). */
uint8_t BspSpi1_Transfer(uint8_t byte);

/* Starts a DMA write and returns at once. data must stay unchanged until done() ran;
 * done() is called from the DMA interrupt once the last bit left the shifter.
 * Must not be called while BspSpi1_DmaBusy(). */
void BspSpi1_WriteDma(const uint8_t *data, uint32_t len, void (*done)(void));

/* Starts a DMA read of len bytes (0xFF clocked out) and returns at once; done() is
 * called from the DMA interrupt once the last byte is in data.
 * Must not be called while BspSpi1_DmaBusy(). */
void BspSpi1_ReadDma(uint8_t *data, uint32_t len, void (*done)(void));

/* A DMA transfer is running */
uint8_t BspSpi1_DmaBusy(void);

/* Called (DMA interrupt) after every DMA transfer that left the bus idle */
void BspSpi1_SetIdleHook(void (*hook)(void));

/* Busy wait until the last written byte is shifted out (< 3 us after BspSpi1_Write) */
void BspSpi1_WaitIdle(void);

#endif /* BSP_SPI1_H */
